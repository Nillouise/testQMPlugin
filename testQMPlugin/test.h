#pragma once
#include<iostream>
#include<sstream>
#include"Room.h"
#include"InstanceZones.h"
#include"Neural.h"
#include"image.h"
using namespace gandalfr;
namespace test
{
	int OpenConsole();
	int getMonsterOverlay(Cdmsoft dm,gandalfr::CRectangle rectSkill);
	int findmonster(Cdmsoft dm);

	
	std::string RectToString(const CRectangle r);

	int InitialNeural();
	int runInsZone(Cdmsoft dm);
	
	int printSetKeyOp();

	int printCurNeural();

	int reset();

	int printImage(Cdmsoft dm);

	int performanceCustomVSdm(Cdmsoft dm);

	int PrintRoomState(Cdmsoft dm);

	int estimateTotalRun(Cdmsoft dm);

	int acturalRun(Cdmsoft dm);

	UINT beginKeyboardThread();
}
