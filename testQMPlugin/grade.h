#pragma once
#include"image.h"
namespace ga //gor grade
{
	const double OneMonster = 10;
	const double NeednMove = 10;// don't need  move
	const double NeednChangeDirection = 3;

	const double moveX = 0.02;// mul with pixel 
	const double moveY = 0.03;

	const DWORD timeActTempToStart= 2000;

	const ima::ColRGB roomMon(0xff,0x00,0x94);
	const ima::ColRGB roomGold(0xdd, 0xdd, 0xdd);//it in no use
	const ima::ColRGB roomObs(0xb5, 0xce, 0xff);
	const ima::ColRGB roomSceneBox(0xff, 0x99, 0xbb);

	const int discardWidth = 3;
	const int discardHeight = 3;
}
