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
	CMonster::findMonster(dm);

	std::vector<std::vector<CRectangle>> receive;
	CDecision::getMonsterOverlay(rectSkill,receive);

//	std::cout << receive[0][0].height;
	std::cout << "Monster Number: " << CMonster::m_vecCMon.size() << std::endl;
	cout << CMonster::m_vecCMon[0].m_rect.x;
	return 0;
}
