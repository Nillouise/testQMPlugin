#include"stdafx.h"
#include "InstanceZones.h"
#include"CharRedEye.h"
#include"Cdmsoft.h"


InsZone g_insZone;

int InsZone::loadNeural()
{
	::InitializeCriticalSection(&CKeyOp::g_csKeyOp);
	RedEye::loadNeural();
	
	return 0;
}

int InsZone::run(Cdmsoft dm)
{
	g_RoomState.run(dm);
	g_selMonster.run();
	
	//it should run the sole neural

	g_action.run();

	return 0;
}
