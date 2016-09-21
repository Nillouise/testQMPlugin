#pragma once
#include"image.h"
namespace ga //gor grade
{
	extern double OneMonster;
	extern double NeednMove;
	extern double NeednChangeDirection;

	extern double moveX;//monster estimate point mul with pixel 
	extern double moveY;//

	extern DWORD timeActTempToStart;//the time when actTemp created,the endtime it should have

	extern const ima::ColRGB roomMon;
	extern const ima::ColRGB roomGold;//it in no use
	extern const ima::ColRGB roomObs;
	extern const ima::ColRGB roomSceneBox;

	extern int discardWidth;// how small object no to considerate .use in RoomState get Room object
	extern int discardHeight;

	extern double imgDigestibility;
	extern double imgDppetite;

	extern int stepX;
	extern int stepY;

	extern DWORD key_ProInterval;

}