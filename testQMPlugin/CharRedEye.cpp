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
namespace RedEye
{
	void simulate()
	{

		ActShuangDao* simuActShuangDao3 = new ActShuangDao();
		simuActShuangDao3->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao3);

		ActShuangDao* simuActShuangDao4 = new ActShuangDao();
		simuActShuangDao4->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao4);

		ActShuangDao* simuActShuangDao5 = new ActShuangDao();
		simuActShuangDao5->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao5);

		ActShuangDao* simuActShuangDao6 = new ActShuangDao();
		simuActShuangDao6->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao6);

		ActShuangDao* simuActShuangDao7 = new ActShuangDao();
		simuActShuangDao7->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao7);

		ActShuangDao* simuActShuangDao8 = new ActShuangDao();
		simuActShuangDao8->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao8);

		ActShuangDao* simuActShuangDao9 = new ActShuangDao();
		simuActShuangDao9->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao9);

		ActShuangDao* simuActShuangDao10 = new ActShuangDao();
		simuActShuangDao10->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao10);

		ActShuangDao* simuActShuangDao11 = new ActShuangDao();
		simuActShuangDao11->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao11);

		ActShuangDao* simuActShuangDao12 = new ActShuangDao();
		simuActShuangDao12->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao12);

		ActShuangDao* simuActShuangDao13 = new ActShuangDao();
		simuActShuangDao13->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao13);

		ActShuangDao* simuActShuangDao14 = new ActShuangDao();
		simuActShuangDao14->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao14);

		ActShuangDao* simuActShuangDao15 = new ActShuangDao();
		simuActShuangDao15->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao15);

		ActShuangDao* simuActShuangDao16 = new ActShuangDao();
		simuActShuangDao16->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao16);

		ActShuangDao* simuActShuangDao17 = new ActShuangDao();
		simuActShuangDao17->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao17);

		ActShuangDao* simuActShuangDao18 = new ActShuangDao();
		simuActShuangDao18->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao18);

		ActShuangDao* simuActShuangDao19 = new ActShuangDao();
		simuActShuangDao19->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao19);

		ActShuangDao* simuActShuangDao20 = new ActShuangDao();
		simuActShuangDao20->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao20);

		ActShuangDao* simuActShuangDao21 = new ActShuangDao();
		simuActShuangDao21->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao21);

		ActShuangDao* simuActShuangDao22 = new ActShuangDao();
		simuActShuangDao22->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao22);

		ActShuangDao* simuActShuangDao23 = new ActShuangDao();
		simuActShuangDao23->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao23);

		ActShuangDao* simuActShuangDao24 = new ActShuangDao();
		simuActShuangDao24->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao24);

		ActShuangDao* simuActShuangDao25 = new ActShuangDao();
		simuActShuangDao25->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao25);

		ActShuangDao* simuActShuangDao26 = new ActShuangDao();
		simuActShuangDao26->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao26);

		ActShuangDao* simuActShuangDao27 = new ActShuangDao();
		simuActShuangDao27->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao27);

		ActShuangDao* simuActShuangDao28 = new ActShuangDao();
		simuActShuangDao28->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao28);

		ActShuangDao* simuActShuangDao29 = new ActShuangDao();
		simuActShuangDao29->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao29);

		ActShuangDao* simuActShuangDao30 = new ActShuangDao();
		simuActShuangDao30->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao30);

		ActShuangDao* simuActShuangDao31 = new ActShuangDao();
		simuActShuangDao31->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao31);

		ActShuangDao* simuActShuangDao32 = new ActShuangDao();
		simuActShuangDao32->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao32);

		ActShuangDao* simuActShuangDao33 = new ActShuangDao();
		simuActShuangDao33->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao33);

		ActShuangDao* simuActShuangDao34 = new ActShuangDao();
		simuActShuangDao34->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao34);

		ActShuangDao* simuActShuangDao35 = new ActShuangDao();
		simuActShuangDao35->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao35);

		ActShuangDao* simuActShuangDao36 = new ActShuangDao();
		simuActShuangDao36->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao36);

		ActShuangDao* simuActShuangDao37 = new ActShuangDao();
		simuActShuangDao37->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao37);

		ActShuangDao* simuActShuangDao38 = new ActShuangDao();
		simuActShuangDao38->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao38);

		ActShuangDao* simuActShuangDao39 = new ActShuangDao();
		simuActShuangDao39->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao39);

		ActShuangDao* simuActShuangDao40 = new ActShuangDao();
		simuActShuangDao40->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao40);

		ActShuangDao* simuActShuangDao41 = new ActShuangDao();
		simuActShuangDao41->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao41);

		ActShuangDao* simuActShuangDao42 = new ActShuangDao();
		simuActShuangDao42->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao42);

		ActShuangDao* simuActShuangDao43 = new ActShuangDao();
		simuActShuangDao43->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao43);

		ActShuangDao* simuActShuangDao44 = new ActShuangDao();
		simuActShuangDao44->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao44);

		ActShuangDao* simuActShuangDao45 = new ActShuangDao();
		simuActShuangDao45->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao45);

		ActShuangDao* simuActShuangDao46 = new ActShuangDao();
		simuActShuangDao46->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao46);

		ActShuangDao* simuActShuangDao47 = new ActShuangDao();
		simuActShuangDao47->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao47);

		ActShuangDao* simuActShuangDao48 = new ActShuangDao();
		simuActShuangDao48->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao48);

		ActShuangDao* simuActShuangDao49 = new ActShuangDao();
		simuActShuangDao49->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao49);

		ActShuangDao* simuActShuangDao50 = new ActShuangDao();
		simuActShuangDao50->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao50);

		ActShuangDao* simuActShuangDao51 = new ActShuangDao();
		simuActShuangDao51->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao51);

		ActShuangDao* simuActShuangDao52 = new ActShuangDao();
		simuActShuangDao52->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao52);

		ActShuangDao* simuActShuangDao53 = new ActShuangDao();
		simuActShuangDao53->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao53);

		ActShuangDao* simuActShuangDao54 = new ActShuangDao();
		simuActShuangDao54->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao54);

		ActShuangDao* simuActShuangDao55 = new ActShuangDao();
		simuActShuangDao55->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao55);

		ActShuangDao* simuActShuangDao56 = new ActShuangDao();
		simuActShuangDao56->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao56);

		ActShuangDao* simuActShuangDao57 = new ActShuangDao();
		simuActShuangDao57->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao57);

		ActShuangDao* simuActShuangDao58 = new ActShuangDao();
		simuActShuangDao58->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao58);

		ActShuangDao* simuActShuangDao59 = new ActShuangDao();
		simuActShuangDao59->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao59);

		ActShuangDao* simuActShuangDao60 = new ActShuangDao();
		simuActShuangDao60->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao60);

		ActShuangDao* simuActShuangDao61 = new ActShuangDao();
		simuActShuangDao61->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao61);

		ActShuangDao* simuActShuangDao62 = new ActShuangDao();
		simuActShuangDao62->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao62);

		ActShuangDao* simuActShuangDao63 = new ActShuangDao();
		simuActShuangDao63->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao63);

		ActShuangDao* simuActShuangDao64 = new ActShuangDao();
		simuActShuangDao64->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao64);

		ActShuangDao* simuActShuangDao65 = new ActShuangDao();
		simuActShuangDao65->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao65);

		ActShuangDao* simuActShuangDao66 = new ActShuangDao();
		simuActShuangDao66->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao66);

		ActShuangDao* simuActShuangDao67 = new ActShuangDao();
		simuActShuangDao67->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao67);

		ActShuangDao* simuActShuangDao68 = new ActShuangDao();
		simuActShuangDao68->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao68);

		ActShuangDao* simuActShuangDao69 = new ActShuangDao();
		simuActShuangDao69->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao69);

		ActShuangDao* simuActShuangDao70 = new ActShuangDao();
		simuActShuangDao70->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao70);

		ActShuangDao* simuActShuangDao71 = new ActShuangDao();
		simuActShuangDao71->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao71);

		ActShuangDao* simuActShuangDao72 = new ActShuangDao();
		simuActShuangDao72->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao72);

		ActShuangDao* simuActShuangDao73 = new ActShuangDao();
		simuActShuangDao73->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao73);

		ActShuangDao* simuActShuangDao74 = new ActShuangDao();
		simuActShuangDao74->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao74);

		ActShuangDao* simuActShuangDao75 = new ActShuangDao();
		simuActShuangDao75->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao75);

		ActShuangDao* simuActShuangDao76 = new ActShuangDao();
		simuActShuangDao76->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao76);

		ActShuangDao* simuActShuangDao77 = new ActShuangDao();
		simuActShuangDao77->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao77);

		ActShuangDao* simuActShuangDao78 = new ActShuangDao();
		simuActShuangDao78->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao78);

		ActShuangDao* simuActShuangDao79 = new ActShuangDao();
		simuActShuangDao79->m_MonToAttack = &g_monNeural2;
		g_AnyToAct[&g_action].insert(simuActShuangDao79);

		ActShuangDao* simuActShuangDao80 = new ActShuangDao();
		simuActShuangDao80->m_MonToAttack = &g_monNeural1;
		g_AnyToAct[&g_action].insert(simuActShuangDao80);


	}

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

	simulate();

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
