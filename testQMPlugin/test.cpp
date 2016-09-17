#include"stdafx.h"
#include "test.h"
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
	CMonsterSet monset =  CMonsterSet::findMonster(dm);

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
	CMonsterSet monset =  CMonsterSet::findMonster(dm);
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
	insZone.loadNeural();

	return 0;
}

int test::runInsZone(Cdmsoft dm)
{
	insZone.run(dm);

	return 0;
}
