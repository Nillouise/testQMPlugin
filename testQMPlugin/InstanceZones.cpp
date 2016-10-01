#include"stdafx.h"
#include "InstanceZones.h"
#include"CharRedEye.h"
#include"Cdmsoft.h"
#include"Visualization.h"
#include"CharBaoPo.h"
InsZone g_insZone;

int InsZone::loadNeural()
{
	BaoPo::loadNeural();
	

	return 0;
}

int InsZone::run(Cdmsoft dm)
{
#ifdef VISUALIZATION
	::EnterCriticalSection(&vis::cs_visualization);
#endif // VISUALIZATION
	g_RoomState.run(dm);
	g_selMonster.run();
	
	//it should run the sole neural

	g_action.run();

#ifdef VISUALIZATION
	::LeaveCriticalSection(&vis::cs_visualization);
#endif // VISUALIZATION

	return 0;
}
