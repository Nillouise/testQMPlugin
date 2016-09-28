#include"stdafx.h"
#include"Decision.h"
#include<vector>
using namespace std;
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
		//change a center skill style  compu to half skill compu 
		int generateHalfSkill(const CRectangle &player, vector<CAttackArea> &attackArea, const CRectangle &skillArea)
		{
			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				auto *area = &it->m_rect;

				//go to right side
				if (player.x +player.width/2 > area->x + area->width / 2)
				{
					area->x = area->x + skillArea.width / 2;
					it->direction = -1;
				}
				else {
					area->x = area->x - skillArea.width / 2;
					it->direction = 1;
				}
			}
			return 0;
		}



		int calAttackAreaScore(const CPlayer &player, vector<CAttackArea> &attackArea, double scOneMonster,double scNeednMove,double scNeednChangeDirection,double scMoveX,double scMoveY ,double AllMonsterWillbeAttack)
		{
			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				it->score = 0;
				CRectangle r;
				//if need move player or change direction ,add the curScore;in other skill,it may hasn't the skill direction
				if (CRectangle::RectCollide(player.m_rect, it->m_rect, &r) == 1)
				{
					it->score += scNeednMove;
					if (abs(it->direction + player.m_direction) > abs(it->direction))
					{
						it->score += scNeednChangeDirection;
					}
				}
				else {
					it->score -= max(abs(r.width)*scMoveX, abs(r.height)*scMoveY);
				}
				it->score += it->num * scOneMonster;
			}
			return 0;
		}




		//work with getMonsterOverlay together
		int selSuitablAttackArea( vector<vector<CRectangle>> &receive,vector<CAttackArea> &generatedAttackArea,double CollidePercentageToEatSmall = 0.55)
		{
			for (auto it = receive.rbegin(); it != receive.rend(); it++)
			{
				for (auto it2 = it->begin(); it2 != it->end(); it2++)
				{
					generatedAttackArea.push_back(CAttackArea(*it2, 0, (receive.rend() - it)));
					for (auto it3 = it + 1; it3 != receive.rend(); it3++)
					{
						for (auto it4 = it3->begin(); it4 != it3->end(); )
						{
							//calculate the percentage the it2 in it4;
							CRectangle r;
							if (CRectangle::RectCollide(*it2, *it4, &r) == 0)
							{
								it4++;
								continue;
							}
							double perArea = (double)(r.width*r.height) / it2->height*it2->width;

							if (perArea > CollidePercentageToEatSmall) {
								it4 = it3->erase(it4);
							}
							else {
								it4++;
							}
						}
					}
				}
			}
			return 0;
		}

		int getNearMonster(const CPlayer &player, CMonsterSet &monsterset)
		{

			return 0;
		}






	}





}