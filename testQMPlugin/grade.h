#pragma once
#include"image.h"
namespace ga //gor grade
{
	extern const double OneMonster;
	extern const double NeednMove;
	extern const double NeednChangeDirection;

	extern const double moveX;// mul with pixel 
	extern const double moveY;

	extern const DWORD timeActTempToStart;

	extern const ima::ColRGB roomMon;
	extern const ima::ColRGB roomGold;//it in no use
	extern const ima::ColRGB roomObs;
	extern const ima::ColRGB roomSceneBox;

	extern const int discardWidth;// how small object no to considerate .use in RoomState get Room object
	extern const int discardHeight;

	extern const double imgDigestibility;
	extern const double imgDppetite;

}