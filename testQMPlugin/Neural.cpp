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

	double maxWeight = DBL_MIN;
	Neural* newAct = NULL;
	for (auto it = actTemp.begin(); it != actTemp.end(); it++)//find the biggest weight
	{
		(*it)->run();
		auto itAct = g_weight.find(make_pair((void*)this, (void*)(*it)));
		double curWeight = 0;
		if (itAct != g_weight.end())
		{
			curWeight = (*it)->m_output * itAct->second;
		}
		if (curWeight > maxWeight)
		{
			maxWeight = curWeight;
			newAct = (*it);
		}
	}



	for (auto it = act.begin(); it != act.end(); it++)//find the biggest weight
	{
		(*it)->run();

		auto itAct = g_weight.find(make_pair((void*)this, (void*)(*it)));
		double curWeight = 0;
		if (itAct != g_weight.end())
		{
			curWeight = (*it)->m_output * itAct->second;
		}
		if (curWeight > maxWeight)
		{
			maxWeight = curWeight;
			newAct = (*it);
		}

	}

	//run the newAct ' trail or keypress
	//if ActNeural run,clear the ActTemp after end();
 	if (newAct != m_curAct )
	{
		m_curAct->end();
		m_curAct = newAct;
		if (typeid(ActTemp) != typeid(newAct->getClassType()))
		{
			for (auto it = g_AnyToActTemp[(void*)this].begin(); it != g_AnyToActTemp[(void*)this].end(); it++)
			{
				delete (*it);
			}
			g_AnyToActTemp[(void*)this].clear();
			m_hisActNeural.push_back((ActNeural*)newAct);
		}
		newAct->express();
	}
	

}

//take a trail ,immediatly run ,and return the end time
DWORD CAction::executeTrail(const vector<CTrail>& trail)
{
	//this function have a problem that it not fit the real keyboard ,because it hasn't the correctly press interval abstract

	//it shounld only delete the run state key
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	CKeyOp::m_setKeyOp.clear();
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

	int runOrWalk = 0;

	int runLength = 0;
	for (auto it = trail.begin(); it != trail.end(); it++)
	{
		runLength += it->x;
		if (abs(runLength) > 300)
		{
			runOrWalk = 1;
			break;
		}
	}

	map<wstring, int> runState(g_RoomState.m_runState);

	DWORD nowTime = GetTickCount();
	for (auto it = trail.begin(); it != trail.end(); it++)
	{
		DWORD curTimeX = playerRunX((*it).x, runState, g_RoomState.m_player.m_speed, nowTime, runOrWalk);
		DWORD curTimeY = playerRunY((*it).y, runState, g_RoomState.m_player.m_speed, nowTime);
		nowTime += max(curTimeX, curTimeY);
	}

	return nowTime;


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


//runOrWalk 1 to must be Run
DWORD CAction::playerRunX(int x, map<wstring,int> &runState,const CSpeed &speed,const DWORD &beginTime,int runOrWalk )
{
	wstring CurDirection=L"";
	wstring NextDirection = L"";


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

	if (runState[ L"left" ] >= 1 )
	{
		CurDirection = L"left";
	}
	if(runState[L"right"] >= 1)
	{
		CurDirection = L"right";
	}


	if (CurDirection == L"")
	{
		if (abs(x) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, 0));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return 7 * 1000;
		}
		if (abs(x) > walk || runOrWalk == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, 0));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (abs(x) > noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 1;
			return curTime;
		}

	}


	if (CurDirection == NextDirection)
	{
		if (runState[CurDirection] == 2 && abs(x)>runForever)
		{
			return 7 * 1000;
		}
		if (runState[CurDirection] == 2)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (runState[CurDirection] == 1 && (abs(x) > walk || runOrWalk == 1))
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, 2));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, 0));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime+ secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (runState[CurDirection] == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
	}

	if (CurDirection != NextDirection)
	{
		if (abs(x)>runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, 2));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, 0));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return 7 * 1000;
		}

		if (abs(x)>walk || runOrWalk == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, 2));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, 0));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (abs(x)>noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, 2));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 1;
			return curTime;
		}

	}

	return 0;
}

DWORD CAction::playerRunY(int y, map<wstring, int> &runState, const CSpeed &speed, const DWORD &beginTime)
{
	wstring curDirection = L"";
	wstring nextDirection = L"";

	const int runForever = 600;
	const int noMove = 0;


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

	if (runState[L"up"] >= 1)
	{
		curDirection = L"up";
	}
	if (runState[L"right"] >= 1)
	{
		curDirection = L"right";
	}

	if (curDirection == L"")
	{
		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return 7 * 1000;
		}

		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}
	}

	if (curDirection == nextDirection)
	{
		if (abs(y) > runForever)
		{
			return 7 * 1000;
			runState[nextDirection] = 2;
		}
		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}

	}

	if (curDirection != nextDirection)
	{
		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		CKeyOp::m_setKeyOp.insert(CKeyOp(curDirection, beginTime , 2));
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, 1));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return 7 * 1000;
		}
		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, 1));
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, 2));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}


	}

	

	return 0;
}

void ActTemp::run()
{
	m_output = m_fnOutput(m_beginTime, m_endTime);
}

void ActTemp::express()
{
	if (m_trail.empty() == true)
	{
		//generate it's only keySignal
		int signal = 2;// the least number 
		for (;; signal++)
		{
			int ok = 1;
			for (auto iter = CKeyOp::m_setKeyOp.begin(); iter != CKeyOp::m_setKeyOp.end(); iter++)
			{
				if (iter->m_signal == signal)
				{
					ok = 0;
					break;
				}
			}
			if (ok == 1)
			{
				m_keySignal = signal;
				break;
			}
		}
		

		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		for (auto it = m_key.begin(); it != m_key.end(); it++)
		{
			it->m_signal = m_keySignal;
			CKeyOp::m_setKeyOp.insert(*it);
		}
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
	}
	else {
		m_endTime = CAction::executeTrail( m_trail );
	}


}


//
void ActTemp::end()
{
	CKeyOp::delKeyNoExe(m_keySignal);
	CKeyOp::upKeyNoUp(m_keySignal);
}

