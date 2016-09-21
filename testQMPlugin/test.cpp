#include"stdafx.h"
#include "test.h"
#include<iostream>
#include<iomanip>
using namespace gandalfr;
using namespace std;
int test::OpenConsole()
{
	AllocConsole();
	freopen("conout$", "w", stdout);
	std::cout << "i'm cout" << std::endl;
	return 0;
}

int test::getMonsterOverlay(Cdmsoft dm, CRectangle rectSkill)
{
	CMonsterSet monset =  CMonsterSet::getMonsterSet(dm);

	std::vector<std::vector<CRectangle>> receive;
	CDecision::getMonsterOverlay(rectSkill,receive, monset);

	cout <<"Layer number:"<< receive.size()<<endl;
	for (auto iter = receive.rbegin(); iter != receive.rend(); iter++)
	{
		cout << "layer"<< receive.size() -  (iter-receive.rbegin()) << endl;
		for (auto iterj = (*iter).begin(); iterj != (*iter).end(); iterj++)
		{
			cout << RectToString((*iterj)) << endl;
		}
	}

	return 0;
}

int test::findmonster(Cdmsoft dm)
{
	CMonsterSet monset =  CMonsterSet::getMonsterSet(dm);
	std::cout << "Monster Number: " << monset.m_vecCMon.size() << std::endl;
	for (auto iter = monset.m_vecCMon.begin(); iter != monset.m_vecCMon.end(); ++iter)
	{
		cout << RectToString((*iter).m_rect) << endl;
	}
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
	g_insZone.run(dm);

	return 0;
}

int test::printSetKeyOp()
{
	auto &aaa = CKeyOp::m_setKeyOp;
	for (auto iter = CKeyOp::m_setKeyOp.begin(); iter != CKeyOp::m_setKeyOp.end(); iter++)
	{
		wcout << iter->m_Key << L"\tkeyType:" << iter->m_KeyType  <<L"\ttime:"<< (((iter->m_KeyTime) / 100 % 600) / 10.0) <<"\tsignal:"<< iter->m_signal << endl;
	}
	return 0;
}

int test::printCurNeural()
{
	if (g_action.m_curActNeural != NULL)
		cout << "curNeural: " << typeid (*((g_action.m_curActNeural)->getClassType())).name() << " output:" << g_action.m_curActNeural->m_output << endl;
	else
		cout << "NULL" << endl;
	return 0;
}

int test::reset()
{
	g_action.m_curActNeural = NULL;

	return 0;
}

int test::printImage(Cdmsoft dm)
{

	ima::CBlock b;
	vector<ima::ColRGB> vColor;
	ima::ColRGB col1(0x3a,0x6e,0xa5);
	vColor.push_back(col1);


	ima::getNewScreen(dm);
	b.getBlock(vColor, set<ima::CBlock>());

	return 0;
}

int test::performanceCustomVSdm(Cdmsoft dm)
{
	ima::CBlock b;
	vector<ima::ColRGB> vColor;
	vColor.push_back(ima::ColRGB(0xff, 0x00, 0x94));

	DWORD curTime = GetTickCount();
	size_t i;
	for (i = 0; i < 1; i++)
	{
		ima::getNewScreen(dm);
		set<ima::CBlock> receive;
		b.getBlock(vColor, receive);

		if (i == 0)
		{
			for (auto iter = receive.begin(); iter != receive.end(); iter++)
			{
				cout<<dec << setw(4) << setfill(' ') << iter->x <<dec << setw(4) << setfill(' ') <<dec << iter->y<<dec << setw(4) << setfill(' ') << iter->width <<dec << setw(4) << setfill(' ') << iter->height<<"  "<< setw(2)<< setfill('0')<< hex<<(int) iter->m_color.col[ima::ColRGB::R] << setw(2) << setfill('0') << hex << (int)iter->m_color.col[ima::ColRGB::G] << setw(2) << setfill('0') <<hex << (int)iter->m_color.col[ima::ColRGB::B];
				if (distance(receive.begin(), iter) % 3 == 2)
				{
					cout << endl;
				}
				else {
					cout << "   ";
				}
			}
			cout << endl;
		}
	}
	cout<<"custom color block search function in "<<i<<" call:" << dec << GetTickCount() - curTime << " " << endl;
	curTime = GetTickCount();
	for (size_t i = 0; i < 1; i++)
	{
		dm.FindColorBlockEx(0, 0, 800, 600, L"ff0094", 1.0, 100, 10, 10);
	}
	cout<<"dm's color block search function in " << i << " call:" << dec << GetTickCount() - curTime<<endl;
	return 0;
}

int test::PrintRoomState(Cdmsoft dm)
{

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


	return 0;
}

int test::estimateTotalRun(Cdmsoft dm)
{

	DWORD curTime = ::GetTickCount();
	g_insZone.run(dm);
	cout << ::GetTickCount() - curTime << endl;
	return 0;
}

int test::acturalRun(Cdmsoft dm)
{
	static UINT uId;
	static int oneThread = 0;
	if (oneThread == 0)
	{
		_beginthreadex(NULL, 0, CKeyOp::KeyboardInput, NULL, 0, NULL);
		oneThread = 1;
	}

	

	return 0;
}

UINT test::beginKeyboardThread()
{
	static UINT uId;
	static int oneThread = 0;
	if (oneThread == 0)
	{
		_beginthreadex(NULL, 0, CKeyOp::KeyboardInput, NULL, 0, &uId);
		oneThread = 1;
	}
	return uId;
}

