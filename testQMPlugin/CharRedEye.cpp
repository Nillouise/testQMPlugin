#include"stdafx.h"
#include "CharRedEye.h"
#include "Room.h"
#include"Neural.h"
#include"grade.h"
using namespace std;
using namespace gandalfr;

void RedEye::ActShuangDao::run()
{
	
	CRectangle skillArea(0, 0, 100, 50);
	vector<vector<CRectangle>> receive;
	CDecision::getMonsterOverlay(skillArea, receive, g_RoomState.m_monster);
	auto &player = g_RoomState.m_player;
	// you can cal the current attack monster, nearly monster,and whole monster
	
	vector<CAttackArea> selGolRec;
	for (auto it = receive.cbegin(); it != receive.cend(); it++)
	{
		for (auto it2 = it->begin(); it2 != it->end(); it2++)
		{
			selGolRec.push_back(CAttackArea(*it2, 0, 1 + receive.size() - (it - receive.cend())));
			for (auto it3 = it + 1; it3 != receive.cend(); it3++)
			{
				for (auto it4 = it3->begin(); it4 != it3->end(); it4++)
				{
					//calculate the percentage the it2 in it4;
					CRectangle r;
					if (CRectangle::RectCollide(*it2, *it4, &r) == 0)
					{
						continue;
					}
					double perArea = (double) (r.width*r.height)/ it2->height*it2->width;

					if (perArea > 0.55) {
						it4 = receive[receive.size() - (it3-receive.cbegin() ) ].erase(it4);


					}
				}
			}
		}
	}

	for (auto it = selGolRec.begin(); it != selGolRec.end(); it++)
	{
		auto &area = it->m_rect;
		
		//go to right side
		if (player.m_rect.x > area.x + area.width / 2)
		{
			area.x = area.x + area.width/2 + skillArea.width/2 - 10;
			it->direction = -1;
		}
		else {
			area.x = area.x + area.width / 2 - skillArea.width / 2 + 10;
			it->direction = 1;
		}
	}

	CAttackArea bestArea(CRectangle(), 0, 0);
	double bestScore = DBL_MIN;
	for (auto it = selGolRec.begin(); it != selGolRec.end(); it++)
	{
		double curScore = 0;
		CRectangle r;

		//if need move player or change direction ,add the curScore;
		if (CRectangle::RectCollide(player.m_rect, it->m_rect,&r) == 1)
		{
			curScore += ga::NeednMove;
			if (abs(it->direction + player.m_direction) > abs(it->direction))
			{
				curScore += ga::NeednChangeDirection;
			}
		}
		else {
			curScore -= max(r.width*ga::moveX, r.height*ga::moveY);
		}
		curScore += it->num * ga::OneMonster;


		if (curScore > bestScore)
		{
			bestScore = curScore;
			bestArea = *it;
		}

	}
	m_bestArea = bestArea;
	m_output = bestScore;
	m_output += sumUpWeight(this);
	//add the monster source weight;
	m_output += (*m_MonToAttack)->m_output + g_weight[make_pair(this,*m_MonToAttack)] ;

}







void RedEye::ActShuangDao::express()
{
	CRectangle rectDist;
	ActTemp* actAttack = new ActTemp();
	DWORD nowTime = GetTickCount();
	actAttack->m_beginTime = nowTime;
	actAttack->creator = this;
	if (CRectangle::RectCollide(m_bestArea.m_rect, g_RoomState.m_player.m_rect, &rectDist) == 1)
	{
		//attack immediatly
		actAttack->m_endTime = nowTime + 2000;
		if (isCoDirection(g_RoomState.m_player.m_direction, m_bestArea.direction) == 0)
		{
			if (m_bestArea.direction < 0)
			{
				actAttack->m_key.push_back(CKeyOp(L"left", nowTime, 0));
			}
			else {
				actAttack->m_key.push_back(CKeyOp(L"right", nowTime, 0));
			}
		}
		for (auto iter = m_ShuangDao.m_Key.begin(); iter != m_ShuangDao.m_Key.end(); iter++)
		{
			auto key(*iter);
			key.m_KeyTime = nowTime;
			actAttack->m_key.push_back(key);
		}
		

	}
	else {
		CTrail tra;

		CRectangle::getRectTrail(g_RoomState.m_player.m_rect, m_bestArea.m_rect, tra);
		actAttack->m_trail.push_back(tra);
	}
	g_AnyToActTemp[&g_action].insert(actAttack);

}
