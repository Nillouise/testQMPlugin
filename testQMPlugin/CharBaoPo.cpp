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

		de::calZeroAttackArea(monNeuralAttackArea);
		de::calAttackAreaScoreOnlyMonsterNumber(monNeuralAttackArea, recMonNeuralArea.size(), ga::OneMonster * 2, ga::AttackAllMonster);
		de::calZeroAttackArea(m_area);
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
		de::expressHalfSkill(this);
	}


	int loadNeural()
	{
		AttackSkill *skLongJuanFeng = new AttackSkill();
		skLongJuanFeng->m_area = CRectangle(0, 0, 30, 20);
		skLongJuanFeng->m_cooldown = 3000;
		skLongJuanFeng->m_HitrecoverTime = 450;
		skLongJuanFeng->m_Key.push_back(CKeyOp(L"z", 0, CKeyOp::PRESS, [skLongJuanFeng](DWORD pressTime) {skLongJuanFeng->release(pressTime); return 0.0; }));
		AttackSkill *skPingX = new AttackSkill();
		skPingX->m_area= CRectangle(0, 0, 500, 70);
		skPingX->m_cooldown = 50;
		skPingX->m_HitrecoverTime = 1200;
		skPingX->m_Key.push_back(CKeyOp(L"x", 0, CKeyOp::DOWMAGAIN, [skPingX](DWORD pressTime) {skPingX->release(pressTime); return 0.0; }));
		AttackSkill *skMoDanLianShe = new AttackSkill();
		skMoDanLianShe->m_area = CRectangle(0, 0, 450, 70);
		skMoDanLianShe->m_cooldown = 4000;
		skMoDanLianShe->m_HitrecoverTime = 1400;
		skMoDanLianShe->m_Key.push_back(CKeyOp(L"a", 0, CKeyOp::PRESS, [skMoDanLianShe](DWORD pressTime) {skMoDanLianShe->release(pressTime); return 0.0; }));
		AttackSkill *skDiYan = new AttackSkill();
		skDiYan->m_area = CRectangle(0, 0, 300, 180);
		skDiYan->m_cooldown = 6000;
		skDiYan->m_HitrecoverTime = 700;
		skDiYan->m_Key.push_back(CKeyOp(L"g", 0, CKeyOp::PRESS, [skDiYan](DWORD pressTime) {skDiYan->release(pressTime); return 0.0; }));
		AttackSkill *skBingJingZhiYu = new AttackSkill();
		skBingJingZhiYu->m_area = CRectangle(0, 0, 350, 230);
		skBingJingZhiYu->m_cooldown = 15*1000;
		skBingJingZhiYu->m_HitrecoverTime = 700;
		skBingJingZhiYu->m_Key.push_back(CKeyOp(L"f", 0, CKeyOp::PRESS, [skBingJingZhiYu](DWORD pressTime) {skBingJingZhiYu->release(pressTime); return 0.0; }));
		AttackSkill *skLeiGuangLian = new AttackSkill();
		skLeiGuangLian->m_area = CRectangle(0, 0, 200, 140);
		skLeiGuangLian->m_cooldown = 15*1000;
		skLeiGuangLian->m_HitrecoverTime = 1300;
		skLeiGuangLian->m_Key.push_back(CKeyOp(L"w", 0, CKeyOp::PRESS, [skLeiGuangLian](DWORD pressTime) {skLeiGuangLian->release(pressTime); return 0.0; }));
		AttackSkill *skXuanHuoDun = new AttackSkill();
		skXuanHuoDun->m_area = CRectangle(0, 0, 450, 140);
		skXuanHuoDun->m_cooldown = 5*1000;
		skXuanHuoDun->m_HitrecoverTime = 500;
		skXuanHuoDun->m_Key.push_back(CKeyOp(L"d", 0, CKeyOp::PRESS, [skXuanHuoDun](DWORD pressTime) {skXuanHuoDun->release(pressTime); return 0.0; }));

		//MoDanLianShe
		//	DiYan
		//	BingJingZhiYu
		//	LeiGuangLian
		//	XuanHuoDun


		ActHalfSkill* actLongjuanfeng1 = new ActHalfSkill(skLongJuanFeng);
		ActHalfSkill* actLongjuanfeng2 = new ActHalfSkill(skLongJuanFeng);
		ActHalfSkill* actPingX1 = new ActHalfSkill(skPingX);
		ActHalfSkill* actPingX2 = new ActHalfSkill(skPingX);
		ActHalfSkill* actMoDanLianShe1 = new ActHalfSkill(skMoDanLianShe);
		ActHalfSkill* actMoDanLianShe2 = new ActHalfSkill(skMoDanLianShe);
		ActHalfSkill* actDiYan1 = new ActHalfSkill(skDiYan);
		ActHalfSkill* actDiYan2 = new ActHalfSkill(skDiYan);
		ActHalfSkill* actBingJingZhiYu1 = new ActHalfSkill(skBingJingZhiYu);
		ActHalfSkill* actBingJingZhiYu2 = new ActHalfSkill(skBingJingZhiYu);
		ActHalfSkill* actLeiGuangLian1 = new ActHalfSkill(skLeiGuangLian);
		ActHalfSkill* actLeiGuangLian2 = new ActHalfSkill(skLeiGuangLian);
		ActHalfSkill* actXuanHuoDun1 = new ActHalfSkill(skXuanHuoDun);
		ActHalfSkill* actXuanHuoDun2 = new ActHalfSkill(skXuanHuoDun);


		actLongjuanfeng1->m_MonToConsiderFirst = &g_monNeural1;
		actLongjuanfeng2->m_MonToConsiderFirst = &g_monNeural2;
		actPingX1->m_MonToConsiderFirst = &g_monNeural1;
		actPingX2->m_MonToConsiderFirst = &g_monNeural2;
		actMoDanLianShe1->m_MonToConsiderFirst = &g_monNeural1;
		actMoDanLianShe2->m_MonToConsiderFirst = &g_monNeural2;
		actDiYan1->m_MonToConsiderFirst = &g_monNeural1;
		actDiYan2->m_MonToConsiderFirst = &g_monNeural2;
		actBingJingZhiYu1->m_MonToConsiderFirst = &g_monNeural1;
		actBingJingZhiYu2->m_MonToConsiderFirst = &g_monNeural2;
		actLeiGuangLian1->m_MonToConsiderFirst = &g_monNeural1;
		actLeiGuangLian2->m_MonToConsiderFirst = &g_monNeural2;
		actXuanHuoDun1->m_MonToConsiderFirst = &g_monNeural1;
		actXuanHuoDun2->m_MonToConsiderFirst = &g_monNeural2;



		MonAny* monAny = new MonAny();
		MonAttacking *monAttacking = new MonAttacking();
		monAttacking->m_numToScore = [](int x) {return x*2.0; };

		Neural::makeWeight(actLongjuanfeng1, monAny, 1, 0);
		Neural::makeWeight(actLongjuanfeng2, monAny, 1, 0);
		Neural::makeWeight(actMoDanLianShe1, monAny, 1, 0);
		Neural::makeWeight(actMoDanLianShe2, monAny, 1, 0);  
		Neural::makeWeight(actDiYan1, monAny, 1, 0);
		Neural::makeWeight(actDiYan2, monAny, 1, 0);  
		Neural::makeWeight(actBingJingZhiYu1, monAny, 1, 0);
		Neural::makeWeight(actBingJingZhiYu2, monAny, 1, 0);  
		Neural::makeWeight(actLeiGuangLian1, monAny, 1, 0);
		Neural::makeWeight(actLeiGuangLian2, monAny, 1, 0);  
		Neural::makeWeight(actXuanHuoDun1, monAny, 1, 0);
		Neural::makeWeight(actXuanHuoDun2, monAny, 1, 0);

		Neural::makeWeight(actLongjuanfeng1, monAttacking, 1, 0);
		Neural::makeWeight(actLongjuanfeng2, monAttacking, 1, 0);
		Neural::makeWeight(actMoDanLianShe1, monAttacking, 1, 0);
		Neural::makeWeight(actMoDanLianShe2, monAttacking, 1, 0);
		Neural::makeWeight(actDiYan1, monAttacking, 1, 0);
		Neural::makeWeight(actDiYan2, monAttacking, 1, 0);
		Neural::makeWeight(actBingJingZhiYu1, monAttacking, 1, 0);
		Neural::makeWeight(actBingJingZhiYu2, monAttacking, 1, 0);
		Neural::makeWeight(actLeiGuangLian1, monAttacking, 1, 0);
		Neural::makeWeight(actLeiGuangLian2, monAttacking, 1, 0);
		Neural::makeWeight(actXuanHuoDun1, monAttacking, 1, 0);
		Neural::makeWeight(actXuanHuoDun2, monAttacking, 1, 0);


		Neural::makeWeight(monAny, &g_selMonster, 1);
		Neural::makeWeight(monAttacking, &g_selMonster, 1);

		g_AnyToMon[&g_selMonster].insert(monAny);//Add monster ,it may be all the same in all character
		g_AnyToMon[&g_selMonster].insert(monAttacking);

		g_AnyToAct[&g_action].insert(actLongjuanfeng1);
		g_AnyToAct[&g_action].insert(actLongjuanfeng2);
		g_AnyToAct[&g_action].insert(actPingX1);
		g_AnyToAct[&g_action].insert(actPingX2);
		g_AnyToAct[&g_action].insert(actMoDanLianShe1);
		g_AnyToAct[&g_action].insert(actMoDanLianShe2);
		g_AnyToAct[&g_action].insert(actDiYan1);
		g_AnyToAct[&g_action].insert(actDiYan2);
		g_AnyToAct[&g_action].insert(actBingJingZhiYu1);
		g_AnyToAct[&g_action].insert(actBingJingZhiYu2);
		g_AnyToAct[&g_action].insert(actLeiGuangLian1);
		g_AnyToAct[&g_action].insert(actLeiGuangLian2);
		g_AnyToAct[&g_action].insert(actXuanHuoDun1);
		g_AnyToAct[&g_action].insert(actXuanHuoDun2);



		return 0;
	}

	//the middle neural layer
	int dissociateNeural()
	{



		return 0;
	}
}