#include"stdafx.h"
#include"Decision.h"
#include<vector>
using namespace std;
namespace gandalfr
{
	namespace de
	{
		int getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive, const CMonsterSet &monset)
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
					if (receive[count].size() == 0)
						receive.erase( (receive.end()-1) );
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


		//if return 1,cut successful,0,no cut,2 cut out all,considerate the y axil
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

		//int YaxilCollide(const CRectangle &rectOri, const CRectangle &rectCutter, CRectangle *remainArea = NULL, CRectangle *cuttedArea = NULL)
		//{
		//	int DeleteCutted = 0;
		//	if (cuttedArea == NULL)
		//	{
		//		cuttedArea = new CRectangle(rectOri);
		//		DeleteCutted = 1;
		//	}
		//	int DeleteRemain = 0;
		//	if (remainArea == NULL)
		//	{
		//		remainArea = new CRectangle(rectOri);
		//		DeleteRemain = 1;
		//	}
		//	int r = 0;
		//	if ((rectOri.y + rectOri.height > rectCutter.y && rectOri.y < rectCutter.y) || (rectOri.y< rectCutter.y + rectCutter.height && rectOri.y + rectOri.height >rectCutter.y + rectCutter.height))
		//	{
		//		if (rectOri.x < rectCutter.x + rectCutter.width && rectOri.x + rectOri.width > rectCutter.x)
		//		{
		//			remainArea->width = rectOri.x + rectOri.width - rectCutter.x + rectCutter.width;
		//			remainArea->x = rectCutter.x + rectCutter.width;
		//			cuttedArea->width = rectOri.x + rectOri.width - (remainArea->x + remainArea->width);
		//			cuttedArea->x = 

		//			r = remainArea->width <= 0 ? 2 : 1;
		//		}
		//		if (rectOri.x + rectOri.width > rectCutter.x && rectOri.x<rectCutter.x + rectCutter.width)
		//		{
		//			remainArea->width = rectCutter.x - rectOri.x;
		//			r = remainArea->width <= 0 ? 2 : 1;
		//		}
		//	}




		//	if (DeleteCutted == 1)
		//	{
		//		delete cuttedArea;
		//	}
		//	if (DeleteRemain == 1)
		//	{
		//		delete remainArea;
		//	}
		//	return r;
		//}


		//int AddCuttedAttackAreaWithMonster(vector<CAttackArea> &attackArea,const CMonsterSet &monster,double AvoidOneMonsterScore)
		//{

		//	//check the cutted area have other monster
		//	for (size_t i = 0; i < length; i++)
		//	{

		//	}

		//	return 0;
		//}

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

		int offsetAttackArea(vector<CAttackArea> &attackArea,int xOffset)
		{
			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				if (it->direction > 0)
				{
					it->m_rect.x -= xOffset;
				}
				else {
					it->m_rect.x += xOffset;
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

		int calAttackAreaScoreInMove(vector<CAttackArea> &attackArea,const  CPlayer &player, double scNeednMove, double scNeednChangeDirection, double scMoveX, double scMoveY)
		{
			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				CRectangle r;
				//if need move player or change direction ,add the curScore;in other skill,it may hasn't the skill direction
				if (CRectangle::RectCollide(player.m_rect, it->m_rect,&r) == 1)
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
			}
			return 0;
		}

		int calZeroAttackArea(vector<CAttackArea> &attackArea,double initValue)
		{
			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				it->score = initValue;
			}
			return 0;
		}

		int generateUpDownSideArea(const CRectangle & seed, vector<CRectangle>& receive, int cutedge, CRectangle cutScreen)
		{
			CRectangle up, down;
			up.x = cutScreen.x;
			up.width = cutScreen.width;
			up.y = cutScreen.y;
			up.height = seed.y - cutScreen.y;

			down.x = cutScreen.x;
			down.width = cutScreen.width;
			down.y = seed.y + seed.height;
			down.height = cutScreen.y + cutScreen.height - down.y;

			receive.push_back(up);
			receive.push_back(down);


			return 2;
		}

		int generateConnerSideArea(const CRectangle & seed, vector<CRectangle>& receive, int cutedge, CRectangle cutScreen)
		{

			CRectangle quadrant1, quadrant2, quadrant3, quadrant4;

			quadrant1.x = cutScreen.x;
			quadrant1.width = seed.x - cutScreen.x;
			quadrant1.y = cutScreen.y;
			quadrant1.height = seed.y - cutScreen.y;

			quadrant2.x = seed.x2();
			quadrant2.width = cutScreen.x2() - quadrant2.x;
			quadrant2.y = cutScreen.y;
			quadrant2.height = seed.y - cutScreen.y;

			quadrant3.x = cutScreen.x;
			quadrant3.width = seed.x - cutScreen.x;
			quadrant3.y = seed.y2();
			quadrant3.height = cutScreen.y2() - seed.y2();

			quadrant4.x = seed.x2();
			quadrant4.width = cutScreen.x2() - seed.x2();
			quadrant4.y = seed.y2();
			quadrant4.height = cutScreen.y2() - seed.y2();

			const int widthRange = 3;
			const int heightRange = 5;

			int count = 0;
			if (cutedge == 1)
			{
				if (quadrant1.width > widthRange && quadrant1.height > heightRange)
				{
					count++;
					receive.push_back(quadrant1);
				}
				if (quadrant2.width > widthRange && quadrant2.height > heightRange)
				{
					count++;
					receive.push_back(quadrant2);
				}
				if (quadrant3.width > widthRange && quadrant3.height > heightRange)
				{
					count++;
					receive.push_back(quadrant3);
				}
				if (quadrant4.width > widthRange && quadrant4.height > heightRange)
				{
					count++;
					receive.push_back(quadrant4);
				}
			}
			else {
				count = 4;
				receive.push_back(quadrant1);
				receive.push_back(quadrant2);
				receive.push_back(quadrant3);
				receive.push_back(quadrant4);
			}



			return count;
		}


		int calAttackAreaScoreOnlyMonsterNumber(vector<CAttackArea> &attackArea,int totalMonsterNum, double scOneMonster, double AllMonsterWillbeAttack)
		{

			for (auto it = attackArea.begin(); it != attackArea.end(); it++)
			{
				it->score += it->num*scOneMonster;
				if (it->num >= totalMonsterNum)
				{

					it->score += AllMonsterWillbeAttack;
				}
			}
			return 0;
		}

		int calAreaWithAreaSize(vector<CAttackArea>& Area, double areaSizeBase, double areaScore)
		{
			for (auto iter  = Area.begin(); iter != Area.end(); iter ++)
			{
				iter->score += iter->m_rect.AreaSize() / areaSizeBase * areaScore;
			}

			return 0;
		}

		CAttackArea selBestAttackArea(vector<CAttackArea> areas)
		{
			CAttackArea bestArea;

			for (auto iter = areas.begin(); iter != areas.end(); iter++)
			{
				if (bestArea.score<iter->score)
				{
					bestArea = *iter;
				}
			}
			return bestArea;
		}


		//work with getMonsterOverlay together
		int selSuitablAttackArea( vector<vector<CRectangle>> &receive,vector<CAttackArea> &generatedAttackArea,double CollidePercentageToEatSmall)
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

		int addAt2ToAt1WhenTheyOverlay(vector<CAttackArea> &At1, const  vector<CAttackArea> &At2,double percentage)
		{
			for (auto iter1 = At1.begin(); iter1 != At1.end(); iter1++)
			{
				for (auto iter2 = At2.begin(); iter2 != At2.end(); iter2++)
				{
					CRectangle colRect;
					if (CRectangle::RectCollide(iter1->m_rect, iter2->m_rect, &colRect) == 1)
					{
						double colArea = colRect.width*colRect.height;
						double at1Area = iter1->m_rect.width * iter1->m_rect.height;
						if (colArea / at1Area > percentage)
						{
							iter1->score += iter2->score;
						}
					}
				}
			}
			return 0;
		}

		int runInActWithAreaHalfSkill(ActWithArea *actNeural)
		{
			actNeural->m_selfOutput = actNeural->m_base;
			actNeural->m_area.clear();
			if (actNeural->m_skill == NULL)
			{
				actNeural->m_selfOutput = -DBL_MAX;
				return 0;
			}
			if (actNeural->m_skill->canUse() == false)
				return 0;

			if ((*(actNeural->m_MonToConsiderFirst)) == NULL)
				return 0;

			vector<vector<CRectangle>> recMonNeuralArea;
			de::getMonsterOverlay(actNeural->m_skill->m_area, recMonNeuralArea, (*(actNeural->m_MonToConsiderFirst))->m_Mon);
			vector<vector<CRectangle>> recMonAllArea;
			de::getMonsterOverlay(actNeural->m_skill->m_area, recMonAllArea);
			vector<CAttackArea> monNeuralAttackArea;
			de::selSuitablAttackArea(recMonNeuralArea, monNeuralAttackArea);
			de::selSuitablAttackArea(recMonAllArea, actNeural->m_area);

			de::calZeroAttackArea(monNeuralAttackArea);
			de::calAttackAreaScoreOnlyMonsterNumber(monNeuralAttackArea, recMonNeuralArea.size(), ga::OneMonster, ga::AttackAllMonster);
			de::calZeroAttackArea(actNeural->m_area);
			de::calAttackAreaScoreOnlyMonsterNumber(actNeural->m_area, recMonAllArea.size(), ga::OneMonster * (*actNeural->m_MonToConsiderFirst)->m_necessary, ga::AttackAllMonster);

			de::addAt2ToAt1WhenTheyOverlay(actNeural->m_area, monNeuralAttackArea, 0.9);

			de::generateHalfSkill(g_RoomState.m_Player.m_rect, actNeural->m_area, actNeural->m_skill->m_area);
			de::offsetAttackArea(actNeural->m_area, 20);

			de::calAttackAreaScoreInMove(actNeural->m_area, g_RoomState.m_Player, ga::NeednMove, ga::NeednChangeDirection, ga::moveX, ga::moveY);
			de::SubRoomEdgeScoreX(g_RoomState.m_Player, actNeural->m_area, ga::SubEdgeAttackAreaX);
			actNeural->m_bestArea = de::selBestAttackArea(actNeural->m_area);
			actNeural->m_selfOutput += actNeural->m_bestArea.score;
			return 0;
		}

		int calscoreAfterRun(ActWithArea * actNeural)
		{
			actNeural->m_output = actNeural->m_selfOutput;
			actNeural->m_output += Neural::sumUpRelativeWeight(actNeural);

			//add the monster source weight;
			//it is interesting,in sumUpRelatveWeight,I add all the edge include the all MonNeural.Now, I add again the one of them,it should mul a large coefficient
			if (*actNeural->m_MonToConsiderFirst != NULL)
				actNeural->m_output += ga::coefficient_monNeural * ((*actNeural->m_MonToConsiderFirst)->m_output)  * g_weight[make_pair(actNeural, *actNeural->m_MonToConsiderFirst)];
			return 0;
		}

		int expressHalfSkill(ActWithArea * actNeural)
		{
			CRectangle rectDist;
			ActTemp* actAttack = new ActTemp();
			DWORD nowTime = GetTickCount();
			actAttack->m_beginTime = nowTime;
			actAttack->creator = actNeural;
			actAttack->m_base = 10;
			actAttack->m_fnOutput = ActTemp::fnOutMustRunComplete();
			//it need an generate m_ouput function;

			if (CRectangle::RectCollide(actNeural->m_bestArea.m_rect, g_RoomState.m_Player.m_rect, &rectDist) == 1)
			{
				CKeyOp::fillVecUpRunKeyCurrentTime(actAttack->m_key, 0);//no continue to press run key;
																		//attack immediatly
				actAttack->m_endTime = nowTime + actNeural->m_skill->m_HitrecoverTime;
				if (isCoDirection(g_RoomState.m_Player.m_direction, actNeural->m_bestArea.direction) == 0)
				{
					if (actNeural->m_bestArea.direction < 0)
					{
						actAttack->m_key.push_back(CKeyOp(L"left", nowTime, CKeyOp::PRESS));
					}
					else {
						actAttack->m_key.push_back(CKeyOp(L"right", nowTime, CKeyOp::PRESS));
					}
					nowTime += 70;// turn round ' s delay time
				}


				for (auto iter = actNeural->m_skill->m_Key.begin(); iter != actNeural->m_skill->m_Key.end(); iter++)
				{
					auto key(*iter);
					key.m_KeyTime = nowTime + iter->m_KeyTime;
					actAttack->m_key.push_back(key);
				}
			}
			else {
				CTrail tra;

				CRectangle::getRectTrail(g_RoomState.m_Player.m_rect, actNeural->m_bestArea.m_rect, tra);
				actAttack->m_endTime = actAttack->m_beginTime + ga::timeActTempToStart;
				actAttack->m_trail.push_back(tra);
			}
			g_AnyToActTemp[&g_action].insert(actAttack);


			return 0;
		}

		//y axil doesn't need to sub?
		int SubRoomEdgeScoreX(const CPlayer & player, CAttackArea & attackArea, double Xscore,double Yscore)
		{
			//yÖá£º287, 367
			//	XÖá£º349, 449
			auto &rePlayer = player.m_rect;
			int rightEdge = 800 - 40;
			int leftEdge = 40;
			if (rePlayer.x > 420)
			{
				if (attackArea.m_rect.x+attackArea.m_rect.width > rightEdge)
				{
					attackArea.score -= (double(attackArea.m_rect.x + attackArea.m_rect.width - rightEdge)) / attackArea.m_rect.width *Xscore;
				}
			}
			else if (rePlayer.x < 370)
			{
				if (attackArea.m_rect.x <  leftEdge)
				{
					attackArea.score -= (double(leftEdge - attackArea.m_rect.x )) / attackArea.m_rect.width *Xscore;
				}
			}
			return 0;
		}
		int SubRoomEdgeScoreX(const CPlayer & player, vector<CAttackArea>& attackArea, double Xscore, double Yscore)
		{

			if (player.m_rect.y<390)
			{
				for (auto iter = attackArea.begin(); iter != attackArea.end(); iter++)
				{
					SubRoomEdgeScoreX(player, *iter, Xscore);
				}
			}
			return 0;
		}

		int SubScreenEdgeScoreDwonY(const CPlayer & player, vector<CAttackArea>& attackArea, double Yscore)
		{
			int downEdge = 600 - 40;
			for (auto iter = attackArea.begin(); iter != attackArea.end(); iter++)
			{
				if (iter->m_rect.y2()  > downEdge)
				{
					iter->score -= (double(iter->m_rect.y2()- downEdge)) / iter->m_rect.width *Yscore;
				}
			}

			return 0;
		}



		CRectangle generateAttackEffect(const CPlayer & player, const CRectangle &skill)
		{
			CRectangle r(skill);
			double offset;
			int playerX;
			if (player.m_direction > 0.5)
			{
				offset = 0;
				playerX = player.m_rect.x + player.m_rect.width;
			}
			else if (player.m_direction < -0.5)
			{
				offset = -1;
				playerX = player.m_rect.x;
			}
			else
			{
				offset = -0.5;
				playerX = player.m_rect.x + player.m_rect.width / 2;
			}

			int UpNearY = 0;
			UpNearY = player.m_rect.y - skill.height / 2;
			UpNearY += skill.y;
			int NearX = 0;
			NearX = playerX + offset*skill.width;
			NearX = NearX + (player.m_direction < 0 ? -1 : 1) * skill.x;
			r.x = NearX;
			r.y = UpNearY;

			return r;
		}

		int generateTwoSideArea(const CRectangle &seed, const CRectangle &area,vector<CRectangle> &receive,int cutedge, CRectangle cutScreen )
		{
			CRectangle left;
			left.x = seed.x - area.width;
			left.y = seed.y - area.height / 2;
			left.width = area.width ;
			left.height = seed.height+area.height;

			CRectangle right;
			right.x = seed.x + seed.width;
			right.y = seed.y - area.height / 2;
			right.width = area.width;
			right.height = seed.height + area.height;

			int successfulCreat = 0;
			if (cutedge == 1)
			{
				if (left.x < cutScreen.x)
				{
					left.width = left.x + left.width - cutScreen.x;
					left.x = cutScreen.x;
					if (left.width > 0)
					{
						receive.push_back(left);
						successfulCreat++;
					}
				}
				if (right.x + right.width > cutScreen.x + cutScreen.width)
				{
					right.width = cutScreen.x + cutScreen.width - right.x;
					if (right.width > 0)
					{
						receive.push_back(right);
						successfulCreat++;
					}
				}
			}
			else {
				receive.push_back(left);
				receive.push_back(right);
				successfulCreat = 2;
			}
			return successfulCreat;
		}
		
		int collideWithMonsterSet(const CRectangle &seed,const CMonsterSet& monsters)
		{
			int count = 0;
			for (auto iter  = monsters.m_vecCMon.begin(); iter  != monsters.m_vecCMon.end(); iter++)
			{
				if (CRectangle::RectCollide(iter->m_rect, seed) == 1)
				{
					count++;
				}
			}
			return count;
		}
		int TwoSideMonsterNumber(const CRectangle&seed, const CRectangle &area, const CMonsterSet& monsters, int &left, int &right)
		{
			vector<CRectangle> receive;
			left = 0;
			right = 0;
			generateTwoSideArea(seed, area, receive);
			for (auto iter = receive.begin(); iter != receive.end(); iter++)
			{
				int count = collideWithMonsterSet(*iter, monsters);
				if (iter->x < seed.x)
				{
					left = count;
				}
				else {
					right = count;
				}
			}
			return min(left , right);
		}

		int generateOverlay(const vector<CRectangle> &seed, vector<vector<CRectangle>> &receive)
		{
			receive.push_back(std::vector<CRectangle>());
			for (auto iter = seed.begin(); iter != seed.end(); iter++)
			{
				CRectangle curRect = (*iter);
				receive[0].push_back(curRect);
			}

			//use pre layer to calculate next layer that overlay

			for (int count = 0; ; count++)
			{
				if (receive[count].size() < 2)
				{
					if (receive[count].size() == 0)
						receive.erase((receive.end() - 1));
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


}

}