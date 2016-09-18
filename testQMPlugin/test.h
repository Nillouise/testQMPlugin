#pragma once
#include<iostream>
#include<sstream>
#include"Room.h"
#include"InstanceZones.h"
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
}
