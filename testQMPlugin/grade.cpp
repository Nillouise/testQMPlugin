#include"stdafx.h"
#include"grade.h"
namespace ga
{
	 double OneMonster = 10;
	 double NeednMove = 10;// don't need  move
	 double NeednChangeDirection = 3;

	 double moveX = 0.02;// mul with pixel 
	 double moveY = 0.03;
	 double AttackAllMonster = 2;


	 DWORD timeActTempToStart = 330;

	const ima::ColRGB roomMon(0xff, 0x00, 0x94);
	const ima::ColRGB roomGold(0xdd, 0xdd, 0xdd);//it in no use
	const ima::ColRGB roomObs(0xb5, 0xce, 0xff);
	const ima::ColRGB roomSceneBox(0xff, 0x99, 0xbb);

	int discardWidth = 3;
	int discardHeight = 3;

	double imgDigestibility = 0.8;
	double imgDppetite = 0.4;

	int stepX = 10;
	int stepY = 10;

	DWORD key_ProInterval = 60;

	double coefficient_monNeural = 10;

	DWORD startRunNeedTime = 450;


	ima::ColRGB ColWhite(0xff, 0xff, 0xff);
	ima::ColRGB Col84ffff(0x84, 0xff, 0xff);
	ima::ColRGB imgPlayerH[3][3] = {
		{ Col84ffff,Col84ffff,Col84ffff },
		{ ColWhite,Col84ffff ,ColWhite },
		{ Col84ffff ,Col84ffff ,Col84ffff }
	};
	ima::ColRGB imgPlayerKou[3][3] = {
		{ Col84ffff,Col84ffff,Col84ffff },
		{ Col84ffff,ColWhite ,Col84ffff },
		{ Col84ffff ,Col84ffff ,Col84ffff }
	};
	ima::ColRGB imgPlayerShi[3][3] = {
		{ ColWhite,Col84ffff,ColWhite },
		{ Col84ffff,Col84ffff ,Col84ffff },
		{ ColWhite ,Col84ffff ,ColWhite }
	};
	DWORD timeRunExcuteToAvoidConfilt = 180;
}