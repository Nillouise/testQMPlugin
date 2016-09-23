#include"stdafx.h"
#include "Room.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>
#include<set>
#include"image.h"
#include"grade.h"
using namespace gandalfr;
using namespace std;
CRoomState g_RoomState;
std::vector<CKeyOp> CKeyOp::m_hisCKeyOp; // record the history key
std::set<CKeyOp >  CKeyOp::m_setKeyOp;  // incoming key
std::map<std::wstring, DWORD>  CKeyOp::m_keyRecentProcess;//the key press in the recent time.if is downing,it should be a max time;
std::map<std::wstring, int>  CKeyOp::m_keyStateSignal;//the key is 0 if not down,else int represent the ActTemp's keySignal;
CRITICAL_SECTION CKeyOp::g_csKeyOp;
bool CKeyOp::m_RunTheKeyBoard;
DWORD CKeyOp::m_nowTime;





//reflect to CMonster::m_vecCMon,responsible to CMonster is not empty;
int gandalfr::CDecision::getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive , const CMonsterSet &monset =g_RoomState.m_Monster )
{
	//preprocess the monster collide with rectSkill and  calculate the 1 monster range.
	receive.push_back(std::vector<CRectangle>());
	for ( auto iter =  monset.m_vecCMon.begin(); iter != monset.m_vecCMon.end(); iter++)
	{
		CRectangle curMon = (*iter).m_rect;
		curMon.x = curMon.x - rectSkill.width / 2;
		curMon.width = curMon.width + rectSkill.width;
		curMon.y = curMon.y - rectSkill.height / 2;
		curMon.height = curMon.height + rectSkill.height;
		receive[0].push_back(curMon);
	}

	//use pre layer to calculate next layer that overlay

	for (int count = 0; ; count++)
	{
		if (receive[count].size() < 2)
		{
			break;
		}
		receive.push_back(std::vector<CRectangle>());
		std::set<CRectangle> setRectDerep;

		for (auto iteri = receive[count].begin(); iteri != receive[count].end(); iteri++)
		{
			for (auto iterj = iteri + 1; iterj != receive[count].end(); iterj++)
			{
				CRectangle &i = (*iteri);
				CRectangle &j = (*iterj);
				CRectangle r;
				r.x = max(i.x, j.x);
				r.y = max(i.y, j.y);
				int x = min(i.x + i.width, j.x + j.width);
				int y = min(i.y + i.height, j.y + j.height);
				if (r.x >= x || r.y >= y)
					continue;
				r.width = x - r.x;
				r.height = y - r.y;
				if (setRectDerep.insert(r).second)
				{
					receive[count + 1].push_back(r);
				}
			}
		}

	}
	return 0;
}


bool gandalfr::operator<(const CKeyOp & t1, const CKeyOp & t2)
{
	if (t1.m_KeyTime < t2.m_KeyTime)
		return true;
	else if (t1.m_KeyTime > t2.m_KeyTime)
		return false;
	if (t1.m_Key < t2.m_Key)
		return true;
	else if (t1.m_Key > t1.m_Key)
		return false;
	if (t1.m_KeyType < t2.m_KeyType)
		return true;
	else if (t1.m_KeyType > t2.m_KeyType)
		return false;
	return false;
}

bool gandalfr::operator <(const CRectangle & t1, const CRectangle & t2) 
{
	if (t1.x < t2.x)
		return true;
	else if (t1.x > t2.x)
		return false;

	if (t1.y < t2.y)
		return true;
	else if (t1.y > t2.y)
		return false;

	if (t1.width < t2.width)
		return true;
	else if (t1.width > t2.width)
		return false;
	
	if (t1.height < t2.height)
		return true;
	else if (t1.height > t2.height)
		return false;	
	return false;
}




int gandalfr::CKeyOp::KeyDefaultCallback(DWORD x)
{
	return 0;
}



int gandalfr::CKeyOp::fillVecUpRunKeyCurrentTime(std::vector<CKeyOp>& vec, const DWORD timeToUpKey)
{
	vec.push_back(CKeyOp(L"left", timeToUpKey, CKeyOp::UP));
	vec.push_back(CKeyOp(L"up", timeToUpKey, CKeyOp::UP));
	vec.push_back(CKeyOp(L"right", timeToUpKey, CKeyOp::UP));
	vec.push_back(CKeyOp(L"down", timeToUpKey, CKeyOp::UP));

	return 0;
}

int gandalfr::CKeyOp::UpSlefKeyAnddelKeyNoExe(int signalId)
{
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	for (auto it = CKeyOp::m_setKeyOp.begin(); it != CKeyOp::m_setKeyOp.end(); )
	{
		if (it->m_signal == signalId)
		{
			CKeyOp::m_setKeyOp.erase(it++);
		}
		else {
			it++;
		}
	}
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	return 0;
}

int gandalfr::CKeyOp::upKeyNoUp(int signalId)
{
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	for (auto it = CKeyOp::m_keyStateSignal.begin(); it != CKeyOp::m_keyStateSignal.end(); it++)
	{
		if (it->second == signalId)
		{
			m_setKeyOp.insert(CKeyOp(it->first,0,CKeyOp::UP));
		}
	}
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	return 0;
}

UINT gandalfr::CKeyOp::KeyboardInput(PVOID)
{
	////register the dm ;
	::CoInitialize(NULL);//初始化线程COM库
	Cdmsoft dm;
	dm.CreateDispatch(L"dm.dmsoft");

	////initial the static value
	m_RunTheKeyBoard = true;
	m_setKeyOp.clear();
	m_keyRecentProcess.clear();
	m_keyStateSignal.clear();



	while (m_RunTheKeyBoard)
	{
		::EnterCriticalSection(&g_csKeyOp);
		m_nowTime = ::GetTickCount();
		vector<CKeyOp> generateKey;
		for (auto iter = m_setKeyOp.begin(); iter != m_setKeyOp.end(); )
		{
			if (iter->m_KeyTime <= m_nowTime)
			{
				if (m_nowTime > m_keyRecentProcess[iter->m_Key] + ga::key_ProInterval)
				{
					if (iter->m_KeyType == UP )
					{
						if (m_keyStateSignal[iter->m_Key] > 0)// 0 is up ing.
						{
							processKey(dm, iter->m_Key, iter->m_KeyType, iter->m_signal);
							iter->m_KeyCallback(m_nowTime);
						}
						iter = m_setKeyOp.erase(iter);
						continue;
					}
					else if (iter->m_KeyType == DOWMNOAGAIN)
					{
						if (m_keyStateSignal[iter->m_Key] == 0)
						{
							processKey(dm, iter->m_Key, iter->m_KeyType, iter->m_signal);
							iter->m_KeyCallback(m_nowTime);
						}
						iter = m_setKeyOp.erase(iter);
						continue;
					}
					else if (iter->m_KeyType == PRESS)
					{
						if (m_keyStateSignal[iter->m_Key] > 0)//if it is downing,0 is not downing
						{
							processKey(dm, iter->m_Key, UP, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							if (iter2 != m_setKeyOp.end())
							{
								nextKey.m_KeyTime = iter2->m_KeyTime - 1;
							}
							generateKey.push_back(nextKey);
							iter++;
							continue;
						}
						else {
							processKey(dm, iter->m_Key, DOWMNOAGAIN, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							if (iter2 != m_setKeyOp.end())
							{
								nextKey.m_KeyTime = iter2->m_KeyTime - 1;
							}
							nextKey.m_KeyType = UP;
							generateKey.push_back(nextKey);
							iter = m_setKeyOp.erase(iter);
						}
					}
					else if (iter->m_KeyType == DOWMAGAIN)
					{
						if (m_keyStateSignal[iter->m_Key] >  0)//if it is downing,0 is not downing
						{
							processKey(dm, iter->m_Key, UP, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							if (iter2 != m_setKeyOp.end())
							{
								nextKey.m_KeyTime = iter2->m_KeyTime - 1;
							}
							nextKey.m_KeyType = DOWMNOAGAIN;
							generateKey.push_back(nextKey);
							iter++;
						}
						else {
							processKey(dm, iter->m_Key, DOWMNOAGAIN, iter->m_signal);
							iter = m_setKeyOp.erase(iter);
						}
					}
				}
				else {
					iter++;
				}
			}
			else {
				break;
			}
		}

		for (auto iter = generateKey.begin(); iter != generateKey.end(); iter++)
		{
			m_setKeyOp.insert(*iter);
		}
		::LeaveCriticalSection(&g_csKeyOp);

	}



	::CoUninitialize();//关闭线程的COM库，此函数应和CoInitialize成对使用。
	return 0;
}


//1 to collide,R is the collided Rectangle
int gandalfr::CRectangle::RectCollide(const CRectangle & A, const CRectangle & B, CRectangle * R)
{
	int DelR = 0;
	if (R == NULL)
	{
		R = new CRectangle();
		DelR = 1;
	}
	int collide = 0;
	R->x = max(A.x, B.x);
	R->y = max(A.y, B.y);
	int x = min(A.x + A.width, B.x + B.width);
	int y = min(A.y + A.height, B.y + B.height);
	if (R->x >= x || R->y >= y)
		collide = 0;
	else
		collide = 1;
	R->width = x - R->x;
	R->height = y - R->y;
	if (DelR == 1)
	{
		delete R;
	}

	return collide;
}


//player go to the center of rect 's trail
int gandalfr::CRectangle::getRectTrail(const CRectangle & player, const CRectangle & rect, CTrail & receive)
{

	int px = player.x + player.width / 2;
	int py = player.y + player.height / 2;

	int rx = rect.x + rect.width / 2;
	int ry = rect.y + rect.height / 2;

	receive.x = rx - px;
	receive.y = ry - py;

	return 0;
}

double gandalfr::isCoDirection(double player, double area)
{
	if (player < 0 && area < 0)
		return 1;
	else if (player > 0 && area > 0)
		return 1;
	return 0;
}





void gandalfr::CRoomState::run(Cdmsoft dm)
{
	ima::getNewScreen(dm);
	getAllRectStateInRoom(dm);
	m_Player.m_rect = CRectangle(100,100, 30, 20);

	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	setRunStateCorrectly();
	m_Player.m_direction =  getPlayerDirectionUseKeyOp();
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
}

//use the ima::g_pbCurScreen return the rectState numer;no process the player.
int gandalfr::CRoomState::getAllRectStateInRoom(Cdmsoft dm)
{
	vector<ima::ColRGB> vBlo;
	vBlo.push_back(ga::roomMon);
	vBlo.push_back(ga::roomObs);
	vBlo.push_back(ga::roomSceneBox);
	vBlo.push_back(ga::roomGold);

	set<ima::CBlock> receive;
	ima::CBlock::getBlock(vBlo,receive);

	//clear the tiny object
	for (auto iter = receive.begin(); iter != receive.end(); )
	{
		if (iter->height < ga::discardHeight || iter->width < ga::discardWidth)
		{
			iter = receive.erase(iter);
		}
		else {
			iter++;
		}
	}
	CMonsterSet newMon;
	CObstacleSet newObs;
	CSceneBoxSet newSceneBox;
	CGoldSet newGold;

	//use colorblock to generate relative Object like monster,obstacle,SceneBox,Gold
	int count = 0;
	for (auto iter = receive.begin(); iter != receive.end();iter++ )
	{
		count++;
		if (iter->m_color == ga::roomMon)
		{
			newMon.m_vecCMon.push_back(CMonsterOne (*iter));
		}
		else if (iter->m_color == ga::roomObs)
		{
			newObs.m_vecObstacle.push_back(CObstacleOne(*iter));
		}
		else if (iter->m_color == ga::roomSceneBox)
		{
			newSceneBox.m_vecCSceneBox.push_back(CSceneBoxOne(*iter));
		}
		else if (iter->m_color == ga::roomGold)
		{
			newGold.m_vecGold.push_back(CGoldOne(*iter));
		}
		else {
			count--;
		}
	}
	m_vecMonTrail.push_back(newMon);
	m_Monster = newMon;
	m_vecObstacleTrail.push_back(newObs);
	m_Obstacle = newObs;
	m_vecSceneBoxTrail.push_back(newSceneBox);
	m_SceneBox = newSceneBox;
	m_vecGoldTrail.push_back(newGold);
	m_Gold = newGold;

	return count;
}

DWORD getKeyRunState(const wstring &key,int begin, DWORD &time, int &Up)
{
	auto &hisKey = CKeyOp::m_hisCKeyOp;
	time = 0;
	Up = 1;
	int preDownTime = 0;
	for (int i = begin; i < hisKey.size(); i++)
	{
		if (i == -1)
		{
			continue;
		}
		if (hisKey[i].m_Key == L"left")
		{
			if (hisKey[i].m_KeyType == CKeyOp::DOWMNOAGAIN)
			{
				begin = i;
				preDownTime = time;
				time = hisKey[i].m_KeyTime;
				Up = 0;
			}
			else
			{
				Up = 1;
			}
		}
	}
	return preDownTime;
}

int gandalfr::CRoomState::setRunStateCorrectly()
{
	auto &hisKey = CKeyOp::m_hisCKeyOp;
	auto &state = g_RoomState.m_runState;

	// these is corresponse key ' index
	static int upperKey = -1;
	static int downKey = -1;
	static int leftKey = -1;
	static int rightkey = -1;

	DWORD leftTime = 0;//the lasteast down time;
	int leftUp = 1;//if left key is up,it is 1;
	DWORD preLeftDown =  getKeyRunState(L"left", leftKey, leftTime, leftUp);
	DWORD rightTime = 0;
	int rightUp = 1;//if left key is up,it is 1;
	DWORD preRightDown =  getKeyRunState(L"right", rightkey, rightTime, rightUp);

	if (leftUp == 1)
		state[L"left"] = 0;
	if(rightUp == 1)
		state[L"right"] = 0;
	if (leftTime - preLeftDown < ga::startRunNeedTime && leftUp == 0 && rightTime < preLeftDown)
		state[L"left"] = 2;
	else if (rightTime - preRightDown < ga::startRunNeedTime && rightUp == 0 && leftTime < preRightDown)
		state[L"right"] = 2;
	else if ((leftTime > rightTime &&leftUp == 0) || (leftUp==0 &&rightUp == 1 ))
		state[L"left"] = 1;
	else if ((rightTime > leftTime &&rightUp == 0 )|| (rightUp == 0&& leftUp==1))
		state[L"right"] = 1;


	DWORD upTime = 0;
	int upUp = 1;
	DWORD preUpDown = getKeyRunState(L"up", upperKey, upTime, upUp);
	DWORD downTime = 0;
	int downUp = 1;
	DWORD preDownDown = getKeyRunState(L"down", downKey, downTime, downUp);

	if (upUp == 1)
		state[L"up"] = 0;
	if (downUp == 1)
		state[L"down"] = 0;
	if ((upTime > downTime && upUp == 0)||(upUp==0 &&downUp == 1))
		state[L"up"] = 1;
	else if ((downTime > upTime &&downUp == 0)||(downUp==0&&upUp==1))
		state[L"down"] = 1;



	return 0;
}

double gandalfr::CRoomState::getPlayerDirectionUseKeyOp()
{
	auto &keyOp = CKeyOp::m_hisCKeyOp;

	int leftIndex = -1;
	auto leftType = CKeyOp::DOWMNOAGAIN;
	for (size_t i = keyOp.size()-1 ; i >= 0; i--)
	{
		if (keyOp[i].m_Key == L"left")
		{
			leftIndex = i;
			leftType = keyOp[i].m_KeyType;
			break;
		}
	}

	int rightIndex = -1;
	auto rightType = CKeyOp::DOWMNOAGAIN;
	for (size_t i = keyOp.size() - 1; i >= 0; i--)
	{
		if (keyOp[i].m_Key == L"right")
		{
			rightIndex = i;
			rightType = keyOp[i].m_KeyType;
			break;
		}
	}

	if (leftIndex == -1)
	{
		if (rightIndex == -1)
		{
			return 0.0;
		}
		return 1.0;//right side
	}
	else {
		if (rightIndex == -1)
			return -1.0;//left side
	}

	if (rightType == CKeyOp::DOWMNOAGAIN)
	{
		if (leftType == CKeyOp::DOWMNOAGAIN)
		{
			return keyOp[rightIndex].m_KeyTime > keyOp[leftIndex].m_KeyTime ? 1.0 : -1.0;
		}
		return 1.0;
	}
	else {
		if (leftType == CKeyOp::DOWMNOAGAIN)
		{
			return -1.0;
		}
		else {// this need to find the pre key to decide which side
			//int leftIndex2 = leftIndex;
			//auto leftType2 = CKeyOp::DOWMNOAGAIN;
			//for (size_t i = leftIndex - 1; i >= 0; i--)
			//{
			//	if (keyOp[i].m_Key == L"left")
			//	{
			//		leftIndex2 = i;
			//		leftType2 = keyOp[i].m_KeyType;
			//	}
			//}
			//int rightIndex2 = rightIndex;
			//auto rightType2 = CKeyOp::DOWMNOAGAIN;
			//for (size_t i = rightIndex - 1; i >= 0; i--)
			//{
			//	if (keyOp[i].m_Key == L"right")
			//	{
			//		rightIndex2 = i;
			//		rightType2 = keyOp[i].m_KeyType;
			//	}
			//}
			if (keyOp[leftIndex].m_KeyTime > keyOp[rightIndex].m_KeyTime)
			{
				return -1.0;
			}
			else {
				return 1.0;
			}

		}
	}





	return 0;
}





void gandalfr::CKeyOp::processKey(Cdmsoft dm, const std::wstring & key, const keyMode & mode, const int & signal)
{
	if (mode == DOWMNOAGAIN)
	{
		dm.KeyDownChar(key.c_str());
		m_hisCKeyOp.push_back(CKeyOp(key, m_nowTime, DOWMNOAGAIN));
		m_keyStateSignal[key] = signal;
		m_keyRecentProcess[key] = m_nowTime;
		return;
	}
	if (mode == UP)
	{
		dm.KeyUpChar(key.c_str());
		m_hisCKeyOp.push_back(CKeyOp(key, m_nowTime, UP));
		m_keyStateSignal[key] = 0;
		m_keyRecentProcess[key] = m_nowTime;
		return;
	}

}
