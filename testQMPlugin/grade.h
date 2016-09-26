#pragma once
#include"image.h"
namespace ga //gor grade
{
	extern double OneMonster; //add the score when a ActNeural can hit a mon
	extern double NeednMove;//add the score when a ActNeural didn't need to move
	extern double NeednChangeDirection;//add the score when a ActNeural didn't need to changeDirection

	extern double moveX;//monster estimate point mul with pixel 
	extern double moveY;//

	extern DWORD timeActTempToStart;//the time when actTemp created,the endtime it should have

	extern const ima::ColRGB roomMon;
	extern const ima::ColRGB roomGold;//it in no use
	extern const ima::ColRGB roomObs;
	extern const ima::ColRGB roomSceneBox;

	extern int discardWidth;// how small object no to considerate .use in RoomState get Room object
	extern int discardHeight;

	extern double imgDigestibility;//a larage rect ' s inner small rect,how much the rate that small rect's appear numer should add to the large rect
	extern double imgDppetite;// how much number rate the big rect have can eat the small one; 

	extern int stepX;
	extern int stepY;

	extern DWORD key_ProInterval;//the interal time a key between two operate

	extern double coefficient_monNeural;//to avoid the efficient that the other Monneural that no real effect the actNeural,mul the actual process monNeural efficient 

	extern DWORD startRunNeedTime;//how least time that two down operate it need to begin a runstate 

	extern ima::ColRGB imgPlayerH[3][3];
	extern ima::ColRGB imgPlayerKou[3][3];
	extern ima::ColRGB imgPlayerShi[3][3];
	extern ima::ColRGB ColWhite;
	extern ima::ColRGB Col84ffff;

	extern DWORD timeRunExcuteToAvoidConfilt;//when begin to run ,after two x key press ,after how much time to begin Y key;
}