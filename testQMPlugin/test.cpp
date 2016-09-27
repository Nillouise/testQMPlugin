#include"stdafx.h"
#include "test.h"
#include<iostream>
#include<iomanip>
#include"grade.h"
#include<string>
#include"Visualization.h"
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

int test::testImg(Cdmsoft dm)
{
	ima::getNewScreen(dm,CRectangle(200,200,2,2));
	//for (size_t i = 0; i <= 5; i++)
	//{
	//	for (size_t j = 0; j <= 5; j++)
	//	{
//			auto p = ima::getColorWhole(i, j);
	//		cout <<hex<< (int)*p << " " <<hex<<(int) *(p+1) << " "<<hex << (int)*(p+2) << endl;
	//	}
	//}
	for (size_t i = 0; i < 100; i++)
	{
		auto p = ima::curScreen::g_pbCurScreen + i;
		cout << hex <<(int) *p << " ";
	}


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
//		test::printBestAreaAndPlayer();
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
	static int lastPrintTime = ::GetTickCount();
	::EnterCriticalSection(&cs_testNeuralThread);

	if ( g_action.m_hisActNeural.size()>NeuralSize )
	{
		if (::GetTickCount() - lastPrintTime < 50)
			return 0;
		lastPrintTime = ::GetTickCount();
		auto curNeural = (g_action.m_hisActNeural[g_action.m_hisActNeural.size() - 1].first);
		if (typeid(*(curNeural->getClassType())) == typeid(RedEye::ActZhiChong))
		{
			auto area = ((RedEye::ActZhiChong*)curNeural)->m_bestArea;
			cout << "Num " << g_action.m_hisActNeural.size() << " :" << RectToString(area.m_rect);
		}
		else {
			auto attackArea = ((RedEye::ActShuangDao*) g_action.m_hisActNeural[g_action.m_hisActNeural.size() - 1].first)->m_bestArea;
			cout << "Num " << g_action.m_hisActNeural.size() << " :" << RectToString(attackArea.m_rect) << " direction:" << attackArea.direction << endl;
		}
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
	cout << "player: "<<RectToString(CPlayer::getPlayer().m_rect) << endl;

	return 0;
}

int test::RunToTarget(Cdmsoft dm,int x,int y)
{
	//vector<CTrail> trail;
	//trail.push_back(CTrail(x, y));
	////CAction::executeTrail(trail, CSpeed(), g_RoomState.m_runState);
	//DWORD t;
	//CAction::playerRunX(900, map<wstring, int>(), CSpeed(), ::GetTickCount(), 0, t);
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	CKeyOp::m_setKeyOp.insert(CKeyOp(L"right", ::GetTickCount(),CKeyOp::PRESS));
	CKeyOp::m_setKeyOp.insert(CKeyOp(L"right", ::GetTickCount(),CKeyOp::DOWMNOAGAIN));


	for (auto iter = CKeyOp::m_setKeyOp.begin(); iter != CKeyOp::m_setKeyOp.end(); iter++)
	{
		wcout << iter->m_Key << L" " << iter->m_KeyType << L"\t";
	}


	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	return 0;
}

int test::printRunState()
{
	g_RoomState.setRunStateCorrectly();
	auto state = g_RoomState.m_runState;
	cout << "left: " << state[L"left"] << " right: " << state[L"right"] << " up: " << state[L"up"] << " down: " << state[L"down"] << endl;

	return 0;
}

int test::ExecuteTraiDeviation(Cdmsoft dm)
{
	ofstream out("C:\\code\\dm\\data\\deviation.txt");
	for (int x = 50; x < 351; x += 50)
	{
		for (int y = 50; y < 301; y += 50)
		{
			RunToTarget(dm, -500, -500);
			Sleep(6*1000);
			ima::getNewScreen(dm);
			Sleep(1000);
			auto rect = CPlayer::getPlayer().m_rect;
			out << rect.x <<" "<<rect.y << endl;
			RunToTarget(dm, x, y);
			Sleep(6 * 1000);
			out << x << " " << y << endl;
			ima::getNewScreen(dm);
			Sleep(1000);
			auto rect2 = CPlayer::getPlayer().m_rect;
			out << rect2.x << "\t" << rect2.y << endl;

		}
	}
	out.close();

	return 0;
}

int test::visualization()
{
	static int flagVis = 0;
	if (flagVis == 0)
	{
		_beginthreadex(NULL, 0, vis::showView, NULL, 0, NULL);
		flagVis = 1;
	}


	return 0;
}





