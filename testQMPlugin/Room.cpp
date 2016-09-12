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
int gandalfr::CDecision::getMonsterOverlay(const CRectangle &rectSkill, const CMonsterSet &monset, std::vector<std::vector<CRectangle>> &receive)
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




bool gandalfr::operator<(const CRectangle & t1, const CRectangle & t2)
{

	return t1.x == t2.x? t1.y< t2.y : t1.x<t2.x;
}
