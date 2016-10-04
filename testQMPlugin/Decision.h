#pragma once
#include"stdafx.h"
#include"Room.h"
#include<vector>
#include"Neural.h"
#include"grade.h"
using namespace std;
namespace gandalfr
{
	namespace de
	{
		int getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive, const CMonsterSet &monset = g_RoomState.m_Monster);
		int cutRectX(CRectangle &rectOri, const CRectangle &rectCutter);
		int generateHalfSkill(const CRectangle &player, vector<CAttackArea> &attackArea, const CRectangle &skillArea);
		int calAttackAreaScore(const CPlayer &player, vector<CAttackArea> &attackArea, double scOneMonster, double scNeednMove, double scNeednChangeDirection, double scMoveX, double scMoveY, double AllMonsterWillbeAttack);
		int selSuitablAttackArea(vector<vector<CRectangle>> &receive, vector<CAttackArea> &generatedAttackArea, double CollidePercentageToEatSmall = 0.55);
		int calAttackAreaScoreOnlyMonsterNumber(vector<CAttackArea> &attackArea, int totalMonsterNum, double scOneMonster, double AllMonsterWillbeAttack);
		int addAt2ToAt1WhenTheyOverlay(vector<CAttackArea> &At1, const  vector<CAttackArea> &At2, double percentage);
		int offsetAttackArea(vector<CAttackArea> &attackArea, int xOffset);
		int calAttackAreaScoreInMove(vector<CAttackArea> &attackArea, const  CPlayer &player, double scNeednMove, double scNeednChangeDirection, double scMoveX, double scMoveY);
		CAttackArea selBestAttackArea(vector<CAttackArea> areas);
		int runInActWithAreaHalfSkill(ActWithArea *actNeural);
		int calscoreAfterRun(ActWithArea *actNeural);
		int epressHalfSkill(ActWithArea *actNeural);
		int SubRoomEdgeScore(const CPlayer & player, CAttackArea & attackArea, double Xscore, double Yscore=0.0);
		int SubRoomEdgeScore(const CPlayer & player, vector<CAttackArea> &attackArea, double Xscore, double Yscore = 0.0);

		CRectangle generateAttackEffect(const CPlayer & player, const CRectangle &skill);
	}

}