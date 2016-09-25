#include"stdafx.h"
#include "test.h"
#include<iostream>
#include<iomanip>
using namespace gandalfr;
using namespace std;

int test::gtest_RunTheWholeNeural;
int test::gtest_pauseNeuralThread;
CRITICAL_SECTION test::cs_testNeuralThread;


int test::OpenConsole()
{
	AllocConsole();
	freopen("conout$", "w", stdout);
	std::cout << "i'm cout" << std::endl;
	return 0;
}


std::string test::RectToString(const CRectangle r)
{

	stringstream ss;
	ss<< "x: " << r.x << " y: " << r.y << " width: " << r.width << " height: " << r.height;
	string s;
	getline(ss, s);
	return s;
}

int test::InitialNeural()
{
	g_insZone.loadNeural();

	return 0;
}

int test::runInsZone(Cdmsoft dm)
{
	::EnterCriticalSection(&cs_testNeuralThread);
	g_insZone.run(dm);
	::LeaveCriticalSection(&cs_testNeuralThread);
	return 0;
}

int test::printSetKeyOp()
{
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	for (auto iter = CKeyOp::m_setKeyOp.begin(); iter != CKeyOp::m_setKeyOp.end(); iter++)
	{
		wcout << iter->m_Key << L"\tkeyType:" << iter->m_KeyType  <<L"\ttime:"<< (((iter->m_KeyTime) / 100 % 600) / 10.0) <<"\tsignal:"<< iter->m_signal << endl;
	}
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	return 0;
}

int test::printCurNeural()
{
	auto p = g_action.m_curActNeural;
	if (p != NULL)
	{
		cout << "curNeural: " << typeid (*((p)->getClassType())).name() << " output:" << g_action.m_curActNeural->m_output << endl;
		if (typeid (*((p)->getClassType())) == typeid(RedEye::ActZhiChong))
			cout << RectToString( ((RedEye::ActZhiChong*)p)->m_bestArea.m_rect);
	}
	else
		cout << "NULL" << endl;
	return 0;
}

int test::reset()
{
	::EnterCriticalSection(&cs_testNeuralThread);
	g_action.m_curActNeural = NULL;
	g_AnyToActTemp[&g_action].clear();

	::LeaveCriticalSection(&cs_testNeuralThread);
	return 0;
}




int test::PrintRoomState(Cdmsoft dm)
{
	::EnterCriticalSection(&cs_testNeuralThread);
	cout << "Gold:" << endl;
	for (auto iter = g_RoomState.m_Gold.m_vecGold.begin(); iter != g_RoomState.m_Gold.m_vecGold.end(); iter++)
	{
		cout << test::RectToString(iter->m_rect)<<"\t";
	}
	cout << endl;

	cout << "Monster:" << endl;
	for (auto iter = g_RoomState.m_Monster.m_vecCMon.begin(); iter != g_RoomState.m_Monster.m_vecCMon.end(); iter++)
	{
		cout << test::RectToString(iter->m_rect)<<"\t";
	}
	cout << endl;

	cout << "Obstacle:" << endl;
	for (auto iter = g_RoomState.m_Obstacle.m_vecObstacle.begin(); iter != g_RoomState.m_Obstacle.m_vecObstacle.end(); iter++)
	{
		cout << test::RectToString(iter->m_rect)<<"\t";
	}
	cout << endl;

	cout << "SceneBox:" << endl;
	for (auto iter = g_RoomState.m_SceneBox.m_vecCSceneBox.begin(); iter != g_RoomState.m_SceneBox.m_vecCSceneBox.end(); iter++)
	{
		cout << test::RectToString(iter->m_rect)<<"\t";
	}
	cout << endl;
	cout << "Player:" << endl;

	cout << RectToString(g_RoomState.m_Player.m_rect) << endl;
	::LeaveCriticalSection(&cs_testNeuralThread);

	return 0;
}

int test::estimateTotalRun(Cdmsoft dm)
{

	::EnterCriticalSection(&cs_testNeuralThread);
	DWORD curTime = ::GetTickCount();
	g_insZone.run(dm);
	cout << "run total one round need millisecond " << ::GetTickCount() - curTime << endl;
	::LeaveCriticalSection(&cs_testNeuralThread);

	return 0;
}



UINT test::beginKeyboardThread()
{
	static UINT uId;
	static int oneThread = 0;
	if (oneThread == 0)
	{
		_beginthreadex(NULL, 0, CKeyOp::KeyboardInput, NULL, 0, &uId);
		cout << "begin KyeBoard Thread" << endl;
		oneThread = 1;
	}
	return uId;
}

UINT test::exitKeyBoardThread()
{
	CKeyOp::m_RunTheKeyBoard = false;
	cout << "exit The keyBoard Thread" << endl;

	return 0;
}

UINT test::beginNeuralThread()
{
	static UINT uId;
	if (gtest_RunTheWholeNeural == 0)
	{
		gtest_RunTheWholeNeural = 1;
		_beginthreadex(NULL, 0, ThreadRunWhole, NULL, 0, &uId);
		cout << "begin Neural Thread" << endl;
	}
	return uId;
}

int test::pauseNeuralThread()
{
	cout << "pause Neural thread" << endl;
	gtest_pauseNeuralThread = 1;
	return 0;
}

int test::restartNeuralThread()
{
	cout << "restart Neural Thread" << endl;
	gtest_pauseNeuralThread = 0;
	return 0;
}

int test::exitTheNeuralThread()
{
	gtest_RunTheWholeNeural = 0;
	gtest_pauseNeuralThread = 0;
	return 0;
}

unsigned int __stdcall test::ThreadRunWhole(PVOID pM)
{
	::CoInitialize(NULL);//初始化线程COM库
	Cdmsoft dm;
	dm.CreateDispatch(L"dm.dmsoft");
	gtest_RunTheWholeNeural = 1;
	gtest_pauseNeuralThread = 0;

	while (gtest_RunTheWholeNeural)
	{
		while (gtest_pauseNeuralThread==1)
		{
			Sleep(10);
		}

		::EnterCriticalSection(&cs_testNeuralThread);
		g_insZone.run(dm);
		::LeaveCriticalSection(&cs_testNeuralThread);
		test::printBestAreaAndPlayer();
	}
	::CoUninitialize();
	cout << "exit the neural thread successfully" << endl;
	return 0;
}

int test::initialTest()
{
	::InitializeCriticalSection(&cs_testNeuralThread);
	OpenConsole();
	return 0;
}

int test::printBestAreaAndPlayer()
{
	static int  NeuralSize = 0;
	static int lastPrintTime = 0;
	::EnterCriticalSection(&cs_testNeuralThread);

	if ( g_action.m_hisActNeural.size()>NeuralSize || ::GetTickCount() -  lastPrintTime > 2000)
	{
		auto attackArea = ((RedEye::ActShuangDao*) g_action.m_hisActNeural[g_action.m_hisActNeural.size() - 1].first)->m_bestArea;
		cout<<"Num "<< g_action.m_hisActNeural.size()<<" :" << RectToString(attackArea.m_rect)<<" direction:"<<attackArea.direction << endl;
		cout <<"player "<< RectToString(g_RoomState.m_Player.m_rect) << endl;
		NeuralSize = g_action.m_hisActNeural.size();
		lastPrintTime = GetTickCount();
	}

	::LeaveCriticalSection(&cs_testNeuralThread);
	return 0;
}

int test::testGetPlayer(Cdmsoft dm)
{
	ima::getNewScreen(dm);
	cout<<RectToString( CPlayer::getPlayer().m_rect);

	return 0;
}





