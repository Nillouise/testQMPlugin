#include"stdafx.h"
#include "Room.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>
#include<set>
using namespace gandalfr;
CRoomState g_RoomState;
std::vector<CKeyOp> CKeyOp::m_vecCKeyOp; // record the history key
std::set<CKeyOp >  CKeyOp::m_setKeyOp;  // incoming key
std::map<std::wstring, DWORD>  CKeyOp::m_keyStateDown;//the key press in the recent time.if is downing,it should be a max time;
std::map<std::wstring, int>  CKeyOp::m_keyStateSignal;//the key is 0 if not down,else int represent the ActTemp's keySignal;
CRITICAL_SECTION CKeyOp::g_csKeyOp;

CMonsterSet gandalfr::CMonsterSet::findMonster(Cdmsoft dm, int rangeX, int rangeY, int rangeWidth, int rangeHeight, WCHAR * MonColor, double similar, int PointCount, int monWidth, int monHeight)
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
int gandalfr::CDecision::getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive , const CMonsterSet &monset =g_RoomState.m_monster )
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
	return false;
}

bool gandalfr::operator <(const CRectangle & t1, const CRectangle & t2)
{

	return t1.x == t2.x? t1.y< t2.y : t1.x<t2.x;
}


int gandalfr::CKeyOp::KeyDefaultCallback(int x)
{
	return 0;
}

int gandalfr::CKeyOp::delKeyNoExe(int signalId)
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
			m_setKeyOp.insert(CKeyOp(it->first,0,20));
		}
	}
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
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

int gandalfr::CPlayer::findPlayer(Cdmsoft dm)
{
	return 0;
}

CPlayer gandalfr::CPlayer::getPlayer(Cdmsoft dm)
{
	return CPlayer();
}

CGoldSet gandalfr::CGoldSet::getGoldSet(Cdmsoft dm)
{
	return CGoldSet();
}

CObstacle gandalfr::CObstacle::getObstacle(Cdmsoft dm)
{
	return CObstacle();
}

CSceneBox gandalfr::CSceneBox::getSceneBox(Cdmsoft dm)
{
	return CSceneBox();
}
