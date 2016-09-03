#include"stdafx.h"
#include "Room.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>

using namespace gandalfr;

std::vector<CMonster> CMonster::m_vecCMon;
std::vector<std::vector<CMonster> > CMonster::m_vecCMonTrail;
static std::vector<CKeyOp> m_vecCKeyOp;


long gandalfr::CMonster::findMonster(Cdmsoft dm, int rangeX, int rangeY, int rangeWidth, int rangeHeight, WCHAR * MonColor, double similar, int PointCount, int monWidth, int monHeight)
{

	static DWORD pretime = GetTickCount();
	CString cs = dm.FindColorBlockEx(rangeX, rangeY, rangeWidth, rangeHeight, MonColor, similar, PointCount, monWidth, monHeight);
	long count = dm.GetResultCount(cs);
	int prex = -100;
	int prey = -100;
	gandalfr::CMonster::m_vecCMon.clear();
	for (int i = 0; i < 1; i++)
	{
		VARIANT intX, intY;
		int dm_ret = dm.GetResultPos(cs, i, &intX, &intY);
		if (abs(intX.intVal - prex) < monWidth / 3 && abs(intY.intVal - prey) < monHeight / 3)
		{
			continue;
		}
		prex = intX.intVal;
		prey = intY.intVal;

		m_vecCMon.push_back(CMonster(intX.intVal, intY.intVal));
	}

	return 0;
}


//reflect to CMonster::m_vecCMon,responsible to CMonster is not empty;
int gandalfr::CDecision::getMonsterOverlay(CRectangle rectSkill, std::vector<std::vector<CRectangle>> &receive)
{
	//preprocess the monster collide with rectSkill and  calculate the 1 monster range.
	receive.push_back(std::vector<CRectangle>());
	for (auto iter = CMonster::m_vecCMon.begin(); iter != CMonster::m_vecCMon.end(); iter++)
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
		receive.push_back(std::vector<CRectangle>());
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
				receive[count + 1].push_back(r);
			}
		}
		if (receive[count + 1].size() < 2)
			break;
	}
	return 0;
}
