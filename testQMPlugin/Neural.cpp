#include"stdafx.h"
#include"Neural.h"
#include"Room.h"
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<float.h>

using namespace std;
using namespace gandalfr;

map<pair<void*, void*>, double> g_weight;

map<void*, set<ActNeural*, comp<ActNeural>> >  g_AnyToAct;
map<void*, set<ActTemp*, comp<ActTemp>> >  g_AnyToActTemp;

void CAction::run()
{
	auto &actTemp =  g_AnyToActTemp[(void*)this];
	auto &act = g_AnyToAct[(void*)this];

	double minWeight = DBL_MIN;
	ActNeural* newAct = NULL;

	for (auto it = actTemp.begin(); it != actTemp.end(); it++)//find the biggest weight
	{
		(*it)->run();
		if ((*it)->m_output * g_weight[make_pair((void*)this, (void*)(*it))] > minWeight)
		{
			minWeight = (*it)->m_output * g_weight[make_pair((void*)this, (void*)(*it))];
			newAct = (*it);
		}
	}

	for (auto it = act.begin(); it != act.end(); it++)//find the biggest weight
	{
		(*it)->run();
		if ((*it)->m_output * g_weight[make_pair((void*)this, (void*)(*it))] > minWeight)
		{
			minWeight = (*it)->m_output * g_weight[make_pair((void*)this, (void*)(*it))];
			newAct = (*it);
		}
	}

	//run the newAct ' trail or keypress
	if (newAct != m_curAct || newAct->newExpress == 1)
	{
		m_curAct = newAct;
		g_AnyToActTemp[(void*)this].clear();
		newAct->express();
	}
	

}

DWORD CAction::keyboard(const CTrail& trail)
{
	//this function have a problem that it not fit the real keyboard ,because it hasn't the correctly press interval abstract

	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	CKeyOp::m_pqKeyOp.swap(priority_queue<CKeyOp, vector<CKeyOp>, greater<CKeyOp> >());
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

	int x = playerRunX(trail.x);
	int y = playerRunY(trail.y);
	return max(x, y);


	//DWORD comsumeTime = 0 ;
	//DWORD nowTime = GetTickCount();
	//DWORD firstPress = 37;
	//DWORD firstUp = 46;
	//DWORD firstDown = 33;
	//DWORD secondDown = 300;
	//DWORD secondUp = 33;

	//auto speed = g_RoomState.m_palyer.m_speed;
	//if (g_RoomState.m_keyboard[L"left"] >= 2 && trail.x < 0 )
	//{
	//	DWORD curTime = 0;
	//	if (abs(trail.x) > 800)
	//	{
	//		curTime = 7 * 1000;
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime +secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"left"] >= 2 && trail.x > 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x > 800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp,2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp); 
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x > 300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstDown , 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"right"] >= 2 && trail.x > 0)
	//{
	//	DWORD curTime = 0;
	//	if (abs(trail.x) > 800)
	//	{
	//		curTime = 7 * 1000;
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"right"] >= 2 && trail.x < 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x < -800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x < -300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}

	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstDown, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//// current process the axil  x walk.
	//else if (g_RoomState.m_keyboard[L"left"] >= 1 && trail.x < 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x < -800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x < -300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"left"] >= 1 && trail.x > 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x > 800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x > 300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime  + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"right"] >= 1 && trail.x > 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x > 800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x > 300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//else if (g_RoomState.m_keyboard[L"right"] >= 1 && trail.x < 0)
	//{
	//	DWORD curTime = 0;
	//	if (trail.x < -800)
	//	{
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//		curTime = 7 * 1000;
	//	}
	//	else if (trail.x < -300)
	//	{
	//		curTime = abs(trail.x) * 1000 / speed.m_spRunX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + firstPress, 0));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	else {
	//		curTime = abs(trail.x) * 1000 / speed.m_spWalkX;
	//		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"right", nowTime + firstUp, 2));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + secondDown, 1));
	//		CKeyOp::m_pqKeyOp.push(CKeyOp(L"left", nowTime + curTime + secondUp, 2));
	//		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	//	}
	//	comsumeTime = max(curTime, comsumeTime);
	//}
	//return 0;
}

DWORD CAction::playerRunX(int x)
{
	wstring CurDirection=L"";
	wstring NextDirection = L"";

	const auto &speed = g_RoomState.m_palyer.m_speed;
	DWORD nowTime = GetTickCount();

	DWORD firstPress = 37;
	DWORD firstUp = 46;
	DWORD firstDown = 33;
	DWORD secondDown = 300;
	DWORD secondUp = 33;


	const int runForever = 800;
	const int walk = 250;
	const int noMove = 0;

	if (x < 0)
	{
		NextDirection = L"left";
	}
	else {
		NextDirection = L"right";
	}

	if (g_RoomState.m_keyboard[ L"left" ] >= 1 )
	{
		CurDirection = L"left";
	}
	if(g_RoomState.m_keyboard[L"right"] >= 1)
	{
		CurDirection = L"right";
	}


	if (CurDirection == L"")
	{
		if (abs(x) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstPress, 0));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return 7 * 1000;
		}
		if (abs(x) > walk)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstPress, 0));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (abs(x) > noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}

	}


	if (CurDirection == NextDirection)
	{
		if (g_RoomState.m_keyboard[CurDirection] == 2 && abs(x)>runForever)
		{
			return 7 * 1000;
		}
		if (g_RoomState.m_keyboard[CurDirection] == 2)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (g_RoomState.m_keyboard[CurDirection] == 1 && abs(x) > walk)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(CurDirection, nowTime, 2));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstPress, 0));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime+ secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (g_RoomState.m_keyboard[CurDirection] == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
	}

	if (CurDirection != NextDirection)
	{
		if (abs(x)>runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(CurDirection, nowTime, 2));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstPress, 0));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return 7 * 1000;
		}

		if (abs(x)>walk)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(CurDirection, nowTime, 2));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + firstPress, 0));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (abs(x)>noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(CurDirection, nowTime, 2));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(NextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}

	}

	return 0;
}

DWORD CAction::playerRunY(int y)
{
	wstring curDirection = L"";
	wstring nextDirection = L"";

	const int runForever = 600;
	const int noMove = 0;

	const auto &speed = g_RoomState.m_palyer.m_speed;
	DWORD nowTime = GetTickCount();
	DWORD firstPress = 37;
	DWORD firstUp = 46;
	DWORD firstDown = 33;
	DWORD secondDown = 300;
	DWORD secondUp = 33;


	if (y < 0)
	{
		nextDirection = L"up";
	}
	else {
		nextDirection = L"down";
	}

	if (g_RoomState.m_keyboard[L"up"] >= 1)
	{
		curDirection = L"up";
	}
	if (g_RoomState.m_keyboard[L"right"] >= 1)
	{
		curDirection = L"right";
	}

	if (curDirection == L"")
	{
		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return 7 * 1000;
		}

		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
	}

	if (curDirection == nextDirection)
	{
		if (abs(y) > runForever)
		{
			return 7 * 1000;
		}
		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}

	}

	if (curDirection != nextDirection)
	{
		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		CKeyOp::m_pqKeyOp.push(CKeyOp(curDirection, nowTime , 2));
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return 7 * 1000;
		}
		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + secondDown, 1));
			CKeyOp::m_pqKeyOp.push(CKeyOp(nextDirection, nowTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}


	}

	

	return 0;
}

void ActTemp::express()
{
}

void ActNeural::express()
{
	if (m_trail.x != 0 || m_trail.y != 0)
	{
		CAction::keyboard(m_trail);
	}
	else {
		if (m_key.size() != 0)
		{
			DWORD nowTime = GetTickCount();

			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			for (auto iter = m_key.begin(); iter != m_key.end(); iter++)
			{
				CKeyOp::m_pqKeyOp.push(*iter);
			}
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);


		}
	}
}
