#pragma once
#include<iostream>
#include<sstream>
#include"Room.h"
#include"InstanceZones.h"
#include"Neural.h"
#include"image.h"
#include"CharRedEye.h"
using namespace gandalfr;
namespace test
{
	int OpenConsole();

	
	
	std::string RectToString(const CRectangle r);

	int InitialNeural();
	int runInsZone(Cdmsoft dm);// it also can run while the neural thread is running
	
	int printSetKeyOp();

	int printCurNeural();

	int reset();
	int testImg(Cdmsoft dm);

	int PrintRoomState(Cdmsoft dm);

	int estimateTotalRun(Cdmsoft dm);//a total run need how much time



	extern int gtest_RunTheWholeNeural;
	extern int gtest_pauseNeuralThread;
	UINT beginKeyboardThread();
	UINT exitKeyBoardThread();


	UINT beginNeuralThread();
	int pauseNeuralThread();
	int restartNeuralThread();
	int exitTheNeuralThread();

	unsigned int __stdcall ThreadRunWhole(PVOID pM);
	extern CRITICAL_SECTION cs_testNeuralThread;

	int initialTest();
	int printBestAreaAndPlayer();

	int testGetPlayer(Cdmsoft dm);

	int RunToTarget(Cdmsoft dm, int x, int y);

	int printRunState();

	int ExecuteTraiDeviation(Cdmsoft dm);

	int visualization();


}
