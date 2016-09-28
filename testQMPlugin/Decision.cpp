#include"stdafx.h"
#include"Decision.h"

namespace gandalfr
{
	namespace de
	{
		int getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive, const CMonsterSet &monset = g_RoomState.m_Monster)
		{
			//preprocess the monster collide with rectSkill and  calculate the 1 monster range.
			receive.push_back(std::vector<CRectangle>());
			for (auto iter = monset.m_vecCMon.begin(); iter != monset.m_vecCMon.end(); iter++)
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


		//if return 1,cut successful,0,no cur,2 cut out all,considerate the y axil
		int cutRectX(CRectangle &rectOri, const CRectangle &rectCutter)
		{
			int r = 0;
			if ((rectOri.y+rectOri.height > rectCutter.y && rectOri.y < rectCutter.y) ||(rectOri.y< rectCutter.y+rectCutter.height && rectOri.y+rectOri.height >rectCutter.y+rectCutter.height ))
			{
				if (rectOri.x < rectCutter.x + rectCutter.width && rectOri.x + rectOri.width > rectCutter.x)
				{
					rectOri.width = rectOri.x + rectOri.width - rectCutter.x + rectCutter.width;
					rectOri.x = rectCutter.x + rectCutter.width;
					r = rectOri.width <= 0 ? 2 : 1;
				}
				if (rectOri.x + rectOri.width > rectCutter.x && rectOri.x<rectCutter.x + rectCutter.width)
				{
					rectOri.width = rectCutter.x - rectOri.x;
					r = rectOri.width <= 0 ? 2 : 1;
				}
			}
			return r;
		}




	}





}