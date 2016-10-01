#include"stdafx.h"
#include "CharBaoPo.h"
#include "Room.h"
#include"Neural.h"
#include"grade.h"
using namespace std;
using namespace gandalfr;

namespace BaoPo
{
	ActPingX::ActPingX()
	{
		m_base = 2;
	}
	void ActPingX::run()
	{
		m_selfOutput = m_base;
		m_area.clear();
		if (m_skill == NULL)
		{
			m_selfOutput = -DBL_MAX;
			return;
		}
		if ((*m_MonToConsiderFirst) == NULL)
			return;

		vector<vector<CRectangle>> recMonNeuralArea;
		de::getMonsterOverlay(m_skill->m_area, recMonNeuralArea, (*m_MonToConsiderFirst)->m_Mon);
		vector<vector<CRectangle>> recMonAllArea;
		de::getMonsterOverlay(m_skill->m_area, recMonAllArea);
		vector<CAttackArea> monNeuralAttackArea;
		de::selSuitablAttackArea(recMonNeuralArea, monNeuralAttackArea);
		de::selSuitablAttackArea(recMonAllArea, m_area);

		de::calAttackAreaScoreOnlyMonsterNumber(monNeuralAttackArea, recMonNeuralArea.size(), ga::OneMonster * 2, ga::AttackAllMonster);
		de::calAttackAreaScoreOnlyMonsterNumber(m_area, recMonAllArea.size(), ga::OneMonster, ga::AttackAllMonster);

		de::addAt2ToAt1WhenTheyOverlay(m_area, monNeuralAttackArea, 0.9);

		de::generateHalfSkill(g_RoomState.m_Player.m_rect, m_area, m_skill->m_area);
		de::offsetAttackArea(m_area, 20);

		de::calAttackAreaScoreInMove(m_area, g_RoomState.m_Player, ga::NeednMove, ga::NeednChangeDirection, ga::moveX, ga::moveY);
		m_bestArea = de::selBestAttackArea(m_area);
		m_selfOutput += m_bestArea.score;

	}
	void ActPingX::cal()
	{
		m_output = m_selfOutput;
		m_output += Neural::sumUpRelativeWeight(this);

		//add the monster source weight;
		//it is interesting,in sumUpRelatveWeight,I add all the edge include the all MonNeural.Now, I add again the one of them,it should mul a large coefficient
		if (*m_MonToConsiderFirst != NULL)
			m_output += ga::coefficient_monNeural * ((*m_MonToConsiderFirst)->m_output)  * g_weight[make_pair(this, *m_MonToConsiderFirst)];
	}
	void ActPingX::express()
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
			CKeyOp::fillVecUpRunKeyCurrentTime(actAttack->m_key, 0);//no continue to press run key;
																	//attack immediatly
			actAttack->m_endTime = nowTime + 1200;
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


			for (auto iter = m_skill->m_Key.begin(); iter != m_skill->m_Key.end(); iter++)
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
	void ActHalfSkill::run()
	{
		de::runInActWithAreaHalfSkill(this);
	}

	void ActHalfSkill::cal()
	{
		de::calscoreAfterRun(this);
	}

	void ActHalfSkill::express()
	{
		de::epressHalfSkill(this);
	}


	int loadNeural()
	{
		AttackSkill *skLongJuanFeng = new AttackSkill();
		skLongJuanFeng->m_area = CRectangle(0, 0, 30, 20);
		skLongJuanFeng->m_cooldown = 3000;
		skLongJuanFeng->m_HitrecoverTime = 700;
		skLongJuanFeng->m_Key.push_back(CKeyOp(L"z", 0, CKeyOp::PRESS, [&](DWORD pressTime) {skLongJuanFeng->release(pressTime); return 0.0; }));
		AttackSkill *skPingX = new AttackSkill();
		skPingX->m_area= CRectangle(0, 0, 500, 70);
		skPingX->m_cooldown = 50;
		skPingX->m_HitrecoverTime = 1800;
		skPingX->m_Key.push_back(CKeyOp(L"x", 0, CKeyOp::DOWMAGAIN, [&](DWORD pressTime) {skPingX->release(pressTime); return 0.0; }));
		// [&](DWORD pressTime) { skPingX->release(pressTime); return 0.0; }





		ActHalfSkill* actLongjuanfeng1 = new ActHalfSkill(skLongJuanFeng);
		ActHalfSkill* actLongjuanfeng2 = new ActHalfSkill(skLongJuanFeng);
		ActHalfSkill* actPingX1 = new ActHalfSkill(skPingX);
		ActHalfSkill* actPingX2 = new ActHalfSkill(skPingX);




		actLongjuanfeng1->m_MonToConsiderFirst = &g_monNeural1;
		actLongjuanfeng2->m_MonToConsiderFirst = &g_monNeural2;
		actPingX1->m_MonToConsiderFirst = &g_monNeural1;
		actPingX2->m_MonToConsiderFirst = &g_monNeural2;

		MonAny* monAny = new MonAny();

		Neural::makeWeight(actLongjuanfeng1, monAny, 1, 0);
		Neural::makeWeight(actLongjuanfeng2, monAny, 1, 0);
		Neural::makeWeight(actPingX1, monAny, 1, 0);
		Neural::makeWeight(actPingX2, monAny, 1, 0);

		Neural::makeWeight(monAny, &g_selMonster, 1);


		g_AnyToMon[&g_selMonster].insert(monAny);//Add monster ,it may be all the same in all character


		g_AnyToAct[&g_action].insert(actLongjuanfeng1);
		g_AnyToAct[&g_action].insert(actLongjuanfeng2);
		g_AnyToAct[&g_action].insert(actPingX1);
		g_AnyToAct[&g_action].insert(actPingX2);

		return 0;
	}

}