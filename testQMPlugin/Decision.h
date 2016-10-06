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
		int cutRectX(CRectangle &rectOri, const CRectangle &rectCutter);//it is no use,because i find a better way to avoid stand in monster
		int generateHalfSkill(const CRectangle &player, vector<CAttackArea> &attackArea, const CRectangle &skillArea);
		int calAttackAreaScore(const CPlayer &player, vector<CAttackArea> &attackArea, double scOneMonster, double scNeednMove, double scNeednChangeDirection, double scMoveX, double scMoveY, double AllMonsterWillbeAttack);
		int selSuitablAttackArea(vector<vector<CRectangle>> &receive, vector<CAttackArea> &generatedAttackArea, double CollidePercentageToEatSmall = 0.55);
		int calAttackAreaScoreOnlyMonsterNumber(vector<CAttackArea> &attackArea, int totalMonsterNum, double scOneMonster, double AllMonsterWillbeAttack);
		int calAreaWithAreaSize(vector<CAttackArea> &Area, double areaSizeBase,double areaScore);//when area size equal areaSizeBase,plus areaScore,else plus correspone efficient areaScore;
		int addAt2ToAt1WhenTheyOverlay(vector<CAttackArea> &At1, const  vector<CAttackArea> &At2, double percentage);
		int offsetAttackArea(vector<CAttackArea> &attackArea, int xOffset);
		int calAttackAreaScoreInMove(vector<CAttackArea> &attackArea, const  CPlayer &player, double scNeednMove, double scNeednChangeDirection, double scMoveX, double scMoveY);
		CAttackArea selBestAttackArea(vector<CAttackArea> areas);
		int runInActWithAreaHalfSkill(ActWithArea *actNeural);
		int calscoreAfterRun(ActWithArea *actNeural);
		int expressHalfSkill(ActWithArea *actNeural);
		int SubRoomEdgeScoreX(const CPlayer & player, CAttackArea & attackArea, double Xscore, double Yscore=0.0);
		int SubRoomEdgeScoreX(const CPlayer & player, vector<CAttackArea> &attackArea, double Xscore, double Yscore = 0.0);
		int SubScreenEdgeScoreDwonY(const CPlayer & player, vector<CAttackArea> &attackArea, double Yscore);
		int generateTwoSideArea(const CRectangle &seed, const CRectangle &area, vector<CRectangle> &receive, int cutedge = 1, CRectangle cutScreen = CRectangle(0, 0, 800, 600));
		CRectangle generateAttackEffect(const CPlayer & player, const CRectangle &skill);
		int TwoSideMonsterNumber(const CRectangle&seed, const CRectangle &area, const CMonsterSet& monsters, int &left, int &right);
		int calZeroAttackArea(vector<CAttackArea> &attackArea, double initValue = 0);
		int calAddScoreAttackArea(vector<CAttackArea> &attackArea, double addValue);
		int generateUpDownSideArea(const CRectangle &seed, vector<CRectangle> &receive, int cutedge = 1, CRectangle cutScreen = CRectangle(0, 0, 800, 600));
		int generateConnerSideArea(const CRectangle &seed, vector<CRectangle> &receive, int cutedge = 1, CRectangle cutScreen = CRectangle(0, 0, 800, 600));
		int generateOverlay(const vector<CRectangle> &seed, vector<vector<CRectangle>> &receive);
		int SubRoomEdgeScoreX(const CPlayer & player, vector<CAttackArea>& attackArea, double Xscore, int leftEdge, int rightEdge);

	}

}