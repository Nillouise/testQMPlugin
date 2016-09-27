#include"stdafx.h"
#include "CharRedEye.h"
#include "Room.h"
#include"Neural.h"
#include"grade.h"
using namespace std;
using namespace gandalfr;

RedEye::ActShuangDao::ActShuangDao()
{
	m_base = 2;

}

void RedEye::ActShuangDao::run()
{
	m_selfOutput = m_base;
	CRectangle skillArea(0, 0, 80, 50);
	vector<vector<CRectangle>> receive;
	
	if ((*m_MonToAttack) == NULL)
	{
		return;
	}
	CDecision::getMonsterOverlay( skillArea, receive,(**m_MonToAttack).m_Mon );// it should use MonNeural


	auto &player = g_RoomState.m_Player;
	
	vector<CAttackArea> selGolRec;//it will have correct monster num,direction
	//select the suitable attackArea,delete the dulplicate area; 
	for (auto it = receive.rbegin(); it != receive.rend(); it++)
	{
		for (auto it2 = it->begin(); it2 != it->end(); it2++)
		{
			selGolRec.push_back(CAttackArea(*it2, 0,  (receive.rend() - it)));
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
					double perArea = (double) (r.width*r.height)/ it2->height*it2->width;

					if (perArea > 0.55) {
						it4 = it3->erase(it4);
					}
					else {
						it4++;
					}
				}
			}
		}
	}

	//generate the side skill area instead of the center skill
	for (auto it = selGolRec.begin(); it != selGolRec.end(); it++)
	{
		auto *area = &it->m_rect;
		
		//go to right side
		if (player.m_rect.x > area->x + area->width / 2)
		{
			area->x = area->x + skillArea.width / 2;
			it->direction = -1;
		}
		else {
			area->x = area->x - skillArea.width/2;
			it->direction = 1;
		}
	}

	CAttackArea bestArea(CRectangle(), 0, 0);
	double bestScore = -DBL_MAX;
	for (auto it = selGolRec.begin(); it != selGolRec.end(); it++)
	{
		double curScore = 0;
		CRectangle r;

		//if need move player or change direction ,add the curScore;in other skill,it may hasn't the skill direction
		if (CRectangle::RectCollide(player.m_rect, it->m_rect,&r) == 1)
		{
			curScore += ga::NeednMove;
			if (abs(it->direction + player.m_direction) > abs(it->direction))
			{
				curScore += ga::NeednChangeDirection;
			}
		}
		else {
			curScore -= max(abs(r.width)*ga::moveX, abs(r.height)*ga::moveY);
		}
		curScore += it->num * ga::OneMonster;


		if (curScore > bestScore)
		{
			bestScore = curScore;
			bestArea = *it;
		}

	}
	m_bestArea = bestArea;
	
	m_selfOutput += bestScore;
}

void RedEye::ActShuangDao::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
	
	//add the monster source weight;
	//it is interesting,in sumUpRelatveWeight,I add all the edge include the all MonNeural.Now, I add again the one of them,it should mul a large coefficient
	if(*m_MonToAttack != NULL)
		m_output += ga::coefficient_monNeural * ((*m_MonToAttack)->m_output)  * g_weight[make_pair(this, *m_MonToAttack)];

}


void RedEye::ActShuangDao::express()
{
	CRectangle rectDist;
	ActTemp* actAttack = new ActTemp();
	DWORD nowTime = GetTickCount();
	actAttack->m_beginTime = nowTime;
	actAttack->creator = this;
	actAttack->m_base = 10;
	actAttack->m_fnOutput = ActTemp::fnOutMustRunComplete();
	//it need an generate m_ouput function;
	
	if (CRectangle::RectCollide(m_bestArea.m_rect, g_RoomState.m_Player.m_rect, &rectDist) == 1)
	{
//		CKeyOp::upRunKey(0);//no continue to press run key;
		//attack immediatly
		actAttack->m_endTime = nowTime + 2000;
		if (isCoDirection(g_RoomState.m_Player.m_direction, m_bestArea.direction) == 0)
		{
			if (m_bestArea.direction < 0)
			{
				actAttack->m_key.push_back(CKeyOp(L"left", nowTime, CKeyOp::PRESS));
			}
			else {
				actAttack->m_key.push_back(CKeyOp(L"right", nowTime, CKeyOp::PRESS));
			}
			nowTime += 70;// turn round ' s delay time
		}


		for (auto iter = m_ShuangDao.m_Key.begin(); iter != m_ShuangDao.m_Key.end(); iter++)
		{
			auto key(*iter);
			key.m_KeyTime = nowTime + iter->m_KeyTime;
			actAttack->m_key.push_back(key);
		}
	}
	else {
		CTrail tra;

		CRectangle::getRectTrail(g_RoomState.m_Player.m_rect, m_bestArea.m_rect, tra);
		actAttack->m_endTime = actAttack->m_beginTime + ga::timeActTempToStart;
		actAttack->m_trail.push_back(tra);
	}
	g_AnyToActTemp[&g_action].insert(actAttack);

}

int RedEye::loadNeural()
{
	ActShuangDao *actShuangDaoMon1 = new ActShuangDao();
	actShuangDaoMon1->m_MonToAttack = &g_monNeural1;
	ActShuangDao *actShuangDaoMon2 = new ActShuangDao();
	actShuangDaoMon2->m_MonToAttack = &g_monNeural2;
	ActZhiChong *actZhiChong1 = new ActZhiChong();
	actZhiChong1->m_MonToAttack = &g_monNeural1;
	ActZhiChong *actZhiChong2 = new ActZhiChong();
	actZhiChong2->m_MonToAttack = &g_monNeural2;

	MonAny* monAny = new MonAny();

	Neural::makeWeight(actShuangDaoMon1, monAny, 1,0);
	Neural::makeWeight(actShuangDaoMon2, monAny, 1,0);


	Neural::makeWeight(actShuangDaoMon1, &g_action, 1);
	Neural::makeWeight(actShuangDaoMon2, &g_action, 1);
	Neural::makeWeight(actZhiChong1, &g_action, 1);
	Neural::makeWeight(actZhiChong2, &g_action, 1);

	Neural::makeWeight(monAny, &g_selMonster, 1);


	g_AnyToMon[&g_selMonster].insert(monAny);//Add monster ,it may be all the same in all character

	g_AnyToAct[&g_action].insert(actShuangDaoMon1);
	g_AnyToAct[&g_action].insert(actShuangDaoMon2);
//	g_AnyToAct[&g_action].insert(actZhiChong1);
//	g_AnyToAct[&g_action].insert(actZhiChong2);
	return 0;
}

RedEye::ActZhiChong::ActZhiChong()
{
	m_base = 100;

}

void RedEye::ActZhiChong::run()
{

	m_selfOutput = m_base;
	if ((*m_MonToAttack) == NULL)
	{
		m_selfOutput = 0;
		return;
	}
	if ((*m_MonToAttack)->m_Mon.m_vecCMon.begin() != (*m_MonToAttack)->m_Mon.m_vecCMon.end())
	{
		m_bestArea.m_rect = (*m_MonToAttack)->m_Mon.m_vecCMon[0].m_rect;
	}
	else
	{
		m_selfOutput = 0;
	}

}

void RedEye::ActZhiChong::cal()
{
	m_output = m_selfOutput;
}

void RedEye::ActZhiChong::express()
{
	CTrail tra;
	ActTemp* actAttack = new ActTemp();
	DWORD nowTime = GetTickCount();
	actAttack->m_beginTime = nowTime;
	actAttack->creator = this;
	actAttack->m_base = 10;
	actAttack->m_fnOutput = ActTemp::fnOutMustRunComplete();
	CRectangle::getRectTrail(g_RoomState.m_Player.m_rect, m_bestArea.m_rect, tra);
	actAttack->m_endTime = actAttack->m_beginTime + ga::timeActTempToStart;
	actAttack->m_trail.push_back(tra);
	g_AnyToActTemp[&g_action].insert(actAttack);
}
