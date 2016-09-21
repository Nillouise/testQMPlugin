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
CRoomState g_RoomState;
std::vector<CKeyOp> CKeyOp::m_vecCKeyOp; // record the history key
std::set<CKeyOp >  CKeyOp::m_setKeyOp;  // incoming key
std::map<std::wstring, DWORD>  CKeyOp::m_keyRecentProcess;//the key press in the recent time.if is downing,it should be a max time;
std::map<std::wstring, int>  CKeyOp::m_keyStateSignal;//the key is 0 if not down,else int represent the ActTemp's keySignal;
CRITICAL_SECTION CKeyOp::g_csKeyOp;

CMonsterSet gandalfr::CMonsterSet::getMonsterSet(Cdmsoft dm, int rangeX, int rangeY, int rangeWidth, int rangeHeight, WCHAR * MonColor, double similar, int PointCount, int monWidth, int monHeight)
{

	static DWORD pretime = GetTickCount();
	CString cs = dm.FindColorBlockEx(rangeX, rangeY, rangeWidth, rangeHeight, MonColor, similar, PointCount, monWidth, monHeight);
	long count = dm.GetResultCount(cs);

	//process the m_vecCMon and m_vecCMonTrail
	CMonsterSet cr;
	
	for (int i = 0; i < count; i++)
	{
		VARIANT intX, intY;
		int dm_ret = dm.GetResultPos(cs, i, &intX, &intY);

		int ok = 1;
		for (auto iter = cr.m_vecCMon.begin(); iter != cr.m_vecCMon.end(); iter++)
		{
			if (abs((*iter).m_rect.x - intX.intVal) < monWidth / 2 && abs((*iter).m_rect.y - intY.intVal) < monHeight / 2)
			{
				ok = 0;
				break;
			}
		}
		if (ok == 1)
		{
			cr.m_vecCMon.push_back(CMonsterOne(CRectangle(intX.intVal, intY.intVal)));
		}
	}

	return cr;
}


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

int gandalfr::CKeyOp::upRunKey(DWORD upTime)
{
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

UINT gandalfr::CKeyOp::KeyboardInput(LPVOID)
{
	//it should  register the dm at the head;
	Cdmsoft dm;


	while (true)
	{
		::EnterCriticalSection(&g_csKeyOp);
		int timeToBreak = 1;

		vector<CKeyOp> generateKey;
		for (auto iter = m_setKeyOp.begin(); iter != m_setKeyOp.end(); iter++)
		{
			if (iter->m_KeyTime <= m_nowTime)
			{
				if (m_nowTime > m_keyRecentProcess[iter->m_Key] + ga::key_ProInterval)
				{
					if (iter->m_KeyType == UP )
					{
						if (m_keyStateSignal[iter->m_Key] >= 0)
						{
							processKey(dm, iter->m_Key, iter->m_KeyType, iter->m_signal);
							iter->m_KeyCallback(m_nowTime);
							iter = m_setKeyOp.erase(iter);
							continue;
						}
						else {
							iter = m_setKeyOp.erase(iter);
							continue;
						}
					}
					else if (iter->m_KeyType == DOWMNOAGAIN)
					{
						if (m_keyStateSignal[iter->m_Key] == 0)
						{
							processKey(dm, iter->m_Key, iter->m_KeyType, iter->m_signal);
							iter->m_KeyCallback(m_nowTime);
							iter = m_setKeyOp.erase(iter);
							continue;
						}
						else {
							iter = m_setKeyOp.erase(iter);
							continue;
						}
					}
					else if (iter->m_KeyType == PRESS)
					{
						if (m_keyStateSignal[iter->m_Key] >= 0)//if it is downing
						{
							processKey(dm, iter->m_Key, UP, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							nextKey.m_KeyTime = iter2->m_KeyTime - 1;
							generateKey.push_back(nextKey);
							iter++;
						}
						else {
							processKey(dm, iter->m_Key, DOWMNOAGAIN, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							nextKey.m_KeyTime = iter2->m_KeyTime - 1;
							nextKey.m_KeyType = UP;
							generateKey.push_back(nextKey);
							iter = m_setKeyOp.erase(iter);
						}
					}
					else if (iter->m_KeyType == DOWMAGAIN)
					{
						if (m_keyStateSignal[iter->m_Key] >= 0)//if it is downing
						{
							processKey(dm, iter->m_Key, UP, iter->m_signal);
							CKeyOp nextKey(*iter);
							auto iter2 = iter;
							iter2++;
							nextKey.m_KeyTime = iter2->m_KeyTime - 1;
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

int gandalfr::isCoDirection(double player, double area)
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
}

//return the rectState numer;
int gandalfr::CRoomState::getAllRectStateInRoom(Cdmsoft dm)
{
	ima::getNewScreen(dm);
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

void gandalfr::CRoomState::clearOldState()
{
	


}

CObstacleSet gandalfr::CObstacleSet::getObstacle(Cdmsoft dm, int rangeX, int rangeY, int rangeWidth, int rangeHeight, WCHAR * ObsColor, double similar, int PointCount, int obsWidth, int obsHeight)
{
	static DWORD pretime = GetTickCount();
	CString cs = dm.FindColorBlockEx(rangeX, rangeY, rangeWidth, rangeHeight, ObsColor, similar, PointCount, obsWidth, obsHeight);
	long count = dm.GetResultCount(cs);

	//process the m_vecCMon and m_vecCMonTrail
	CMonsterSet cr;

	for (int i = 0; i < count; i++)
	{
		VARIANT intX, intY;
		int dm_ret = dm.GetResultPos(cs, i, &intX, &intY);

		int ok = 1;
		for (auto iter = cr.m_vecCMon.begin(); iter != cr.m_vecCMon.end(); iter++)
		{
			if (abs((*iter).m_rect.x - intX.intVal) < obsWidth / 2 && abs((*iter).m_rect.y - intY.intVal) < obsHeight / 2)
			{
				ok = 0;
				break;
			}
		}
		if (ok == 1)
		{
			cr.m_vecCMon.push_back(CMonsterOne(CRectangle(intX.intVal, intY.intVal)));
		}
	}

//	return cr;




	return CObstacleSet();
}
