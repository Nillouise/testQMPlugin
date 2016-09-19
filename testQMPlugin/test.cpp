#include"stdafx.h"
#include "test.h"
#include<iostream>
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
	while (true)
	{
		g_insZone.run(dm);
	}
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
		cout << "curNeural: " << typeid (*((g_action.m_curActNeural)->getClassType())).name() << " " << g_action.m_curActNeural->m_output << endl;
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
	vector<byte*> vColor;
	byte by1[3] = { 0x3a,0x6e,0xa5 };
	vColor.push_back(by1);


	ima::getNewScreen(dm);
	b.getBlock(vColor, set<ima::CBlock>());

	return 0;
}

int test::performanceCustomVSdm(Cdmsoft dm)
{
	ima::CBlock b;
	vector<byte*> vColor;
	byte by1[3] = { 0x3a,0x6e,0xa5 };
	vColor.push_back(by1);

	DWORD curTime = GetTickCount();
	for (size_t i = 0; i < 100; i++)
	{
		ima::getNewScreen(dm);
		b.getBlock(vColor, set<ima::CBlock>());
	}
	cout << GetTickCount() - curTime<<" ";
	curTime = GetTickCount();
	for (size_t i = 0; i < 100; i++)
	{
		dm.FindColorBlockEx(0, 0, 800, 600, L"3a6ea5", 1.0, 100, 10, 10);
	}
	cout << GetTickCount() - curTime<<endl;
	return 0;
}

