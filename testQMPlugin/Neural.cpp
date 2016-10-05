#include"stdafx.h"
#include"Neural.h"
#include"Room.h"
#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<float.h>
#include"grade.h"
#include"Decision.h"
using namespace std;
using namespace gandalfr;

map<pair<void*, void*>, double> g_weight;
map<void*, set<ActNeural*, comp<ActNeural>> >  g_AnyToAct;
map<void*, set<ActTemp*, comp<ActTemp>> >  g_AnyToActTemp;
map<void*, set<MonNeural*, comp<MonNeural>> >  g_AnyToMon;
CAction g_action;
SelMonster g_selMonster;
MonNeural* g_monNeural1;
MonNeural* g_monNeural2;



void CAction::run()
{
	
	auto &actTemp =  g_AnyToActTemp[(void*)this];
	auto &act = g_AnyToAct[(void*)this];

	double maxWeight = -DBL_MAX;
	Neural* newAct = NULL;

	//run and cal the linked Neural output;
	for (auto it = actTemp.begin(); it != actTemp.end(); it++)//find the biggest weight
	{
		(*it)->run();
	}

	for (auto it = act.begin(); it != act.end(); it++)//find the biggest weight
	{
		(*it)->run();
	}
	for (size_t i = 0; i < 2; i++)
	{
		for (auto it = actTemp.begin(); it != actTemp.end(); it++)//find the biggest weight
		{
			(*it)->cal();
		}

		for (auto it = act.begin(); it != act.end(); it++)//find the biggest weight
		{
			(*it)->cal();
		}
	}


	for (auto it = actTemp.begin(); it != actTemp.end(); it++)//find the biggest weight
	{
		auto itAct = g_weight.find(make_pair((void*)this, (void*)(*it)));

		//cal the curWeight,because ActTemp Neural has not link to caction,caction should use actTemp actual m_output 
		double curWeight = (*it)->m_output;
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


		auto itAct = g_weight.find(make_pair((void*)this, (void*)(*it)));
		double curWeight = (*it)->m_output;
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
 	if (newAct != m_curActNeural )
	{
		if (m_curActNeural != NULL)
		{
			m_curActNeural->end();
		}
		m_curActNeural = newAct;


		if (typeid(ActTemp) != typeid(*(newAct->getClassType())))// if express a new ActNeural
		{
			for (auto it = g_AnyToActTemp[(void*)this].begin(); it != g_AnyToActTemp[(void*)this].end(); it++)
			{
				delete (*it);
			}
			g_AnyToActTemp[(void*)this].clear();
			m_hisActNeural.push_back( make_pair((ActNeural*)newAct, GetTickCount()));
			m_curActNeural = NULL;

		}
		newAct->express();
	}
	

}

//take a trail ,immediatly run ,and return the end time
DWORD CAction::executeTrail(const vector<CTrail>& trail,const CSpeed &speed,const map<wstring,int> &runState)
{
	CKeyOp::eraseRunKey();

	int runOrWalk = 0;

	int runLength = 0;
	for (auto it = trail.begin(); it != trail.end(); it++)
	{
		int prelength = runLength;
		runLength += it->x;
		if (abs(prelength) > abs(runLength))
			break;
		if (abs(runLength) > 300  )
		{
			runOrWalk = 1;
			break;
		}
	}

	map<wstring, int> cpy_runState(runState);

	DWORD nowTime = GetTickCount()+2000;

	for (auto it = trail.begin(); it != trail.end(); it++)
	{
		DWORD yBeginTime = nowTime;
		DWORD curTimeX = playerRunX((*it).x, cpy_runState, speed, nowTime, runOrWalk, yBeginTime);
//		DWORD curTimeY = 0;
		DWORD curTimeY = playerRunY((*it).y, cpy_runState, speed, yBeginTime);
		curTimeY += yBeginTime - nowTime;
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
//timeToYexcute is to avoid between two x key ,a y key event appear
DWORD CAction::playerRunX(int x, map<wstring,int> &runState,const CSpeed &speed,const DWORD &beginTime,int runOrWalk ,DWORD &timeToYexcute)
{
	wstring CurDirection=L"";
	wstring NextDirection = L"";
	timeToYexcute = beginTime;

	DWORD firstPress = 0;
	DWORD firstUp = 0;
	DWORD firstDown = 0;
	DWORD secondDown = 0;
	DWORD secondUp = 0;


	const int runForever = 800;
	const int walk = 250;
	const int noMove = 30;

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
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, CKeyOp::PRESS));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			timeToYexcute = beginTime + secondDown + ga::timeRunExcuteToAvoidConfilt;
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return 7 * 1000;
		}
		if (abs(x) > walk || runOrWalk == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, CKeyOp::PRESS));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			timeToYexcute = beginTime + secondDown + ga::timeRunExcuteToAvoidConfilt;
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (abs(x) > noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;



			auto &fsdf = CKeyOp::m_setKeyOp;



			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 1;
			return curTime;
		}

	}
	else if (CurDirection == NextDirection)
	{
		if (runState[CurDirection] == 2 && abs(x)>runForever)
		{
			return 7 * 1000;
		}
		if (runState[CurDirection] == 2)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
		if (runState[CurDirection] == 1 && (abs(x) > walk || runOrWalk == 1))
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, CKeyOp::UP));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, CKeyOp::PRESS));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime+ secondUp, CKeyOp::UP));
			timeToYexcute = beginTime + secondDown + ga::timeRunExcuteToAvoidConfilt;
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (runState[CurDirection] == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			return curTime;
		}
	}
	else if (CurDirection != NextDirection)
	{
		if (abs(x)>runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, CKeyOp::UP));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, CKeyOp::PRESS));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			timeToYexcute = beginTime + secondDown + ga::timeRunExcuteToAvoidConfilt;
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return 7 * 1000;
		}

		if (abs(x)>walk || runOrWalk == 1)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spRunX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, CKeyOp::UP));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + firstPress, CKeyOp::PRESS));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			timeToYexcute = beginTime + secondDown + ga::timeRunExcuteToAvoidConfilt;
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[NextDirection] = 2;
			return curTime;
		}
		if (abs(x)>noMove)
		{
			DWORD curTime = abs(x) * 1000 / speed.m_spWalkX;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(CurDirection, beginTime, CKeyOp::UP));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(NextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
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
	const int noMove = 30;


	DWORD firstPress = 0;
	DWORD firstUp = 0;
	DWORD firstDown = 0;
	DWORD secondDown = 0;
	DWORD secondUp = 0;


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
	if (runState[L"down"] >= 1)
	{
		curDirection = L"down";
	}

	if (curDirection == L"")
	{
		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return 7 * 1000;
		}

		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}
	}else if (curDirection == nextDirection)
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
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}

	}
	else if (curDirection != nextDirection)
	{
		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		CKeyOp::m_setKeyOp.insert(CKeyOp(curDirection, beginTime , CKeyOp::UP));
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

		if (abs(y) > runForever)
		{
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return 7 * 1000;
		}
		if (abs(y) > noMove)
		{
			DWORD curTime = abs(y) * 1000 / speed.m_spWalkY;
			::EnterCriticalSection(&CKeyOp::g_csKeyOp);
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + secondDown, CKeyOp::DOWMNOAGAIN));
			CKeyOp::m_setKeyOp.insert(CKeyOp(nextDirection, beginTime + curTime + secondUp, CKeyOp::UP));
			::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
			runState[nextDirection] = 2;
			return curTime;
		}


	}
	return 0;
}


function<double(DWORD begin, DWORD end, Neural *neural)> ActTemp::fnOutMustRunComplete()
{
	auto p = [](DWORD beginTime ,DWORD endTime, Neural *neural)
	{
		double r = 0;
		DWORD nowtime = GetTickCount();
		if (nowtime > beginTime && nowtime < endTime)
		{
			r = 1000;
		}
		return  r;
	};
	return p;
}

function<double(DWORD begin, DWORD end, Neural*neural)> ActTemp::fnOutGiveUpControlInLastXmillisecond(DWORD x)
{
	auto p = [=](DWORD beginTime, DWORD endTime, Neural *neural)
	{
		double r = 0;
		DWORD nowtime = GetTickCount();
		if (nowtime > beginTime && nowtime < endTime-x)
		{
			r = 1000;
		}
		return  r;
	};
	return p;
}


void ActTemp::run()
{
	m_selfOutput = m_base;

	if (m_fnOutput != NULL)
	{
		m_selfOutput += m_fnOutput(m_beginTime, m_endTime,this);
	}
	
}


void ActTemp::express()
{
	//generate it's only keySignal
	int signal = 3;//0 can not be use ,1 is run state,2 is release skill the least number 
	for (;; signal++)
	{
		int ok = 1;
		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		for (auto iter = CKeyOp::m_setKeyOp.begin(); iter != CKeyOp::m_setKeyOp.end(); iter++)
		{
			if (iter->m_signal == signal)
			{
				ok = 0;
				break;
			}
		}
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
		if (ok == 1)
		{
			m_keySignal = signal;
			break;
		}
	}
	if (m_trail.empty() == true)
	{
		::EnterCriticalSection(&CKeyOp::g_csKeyOp);
		for (auto it = m_key.begin(); it != m_key.end(); it++)
		{
			it->m_signal = m_keySignal;
			CKeyOp::m_setKeyOp.insert(*it);
		}
		::LeaveCriticalSection(&CKeyOp::g_csKeyOp);
		
	}
	else {
		m_endTime = CAction::executeTrail( m_trail,g_RoomState.m_Player.m_speed,g_RoomState.m_runState );
	}


}

//it must have correctly Up Key if you use m_key,
void ActTemp::end()
{
	queue<CKeyOp> qKey;
	map<wstring, int> firstKeyUp;

	//find the no up key that this actTemp down
	::EnterCriticalSection(&CKeyOp::g_csKeyOp);
	for (auto iter = CKeyOp::m_keyStateSignal.begin(); iter != CKeyOp::m_keyStateSignal.end();iter++)
	{
		if (iter->second ==  m_keySignal)
		{
			CKeyOp upkey(iter->first);
			upkey.m_KeyTime = GetTickCount();
			upkey.m_KeyType = CKeyOp::UP;
			qKey.push(upkey);
		}
	}
	//delete all key this actTemp press(no include the trail)
	for (auto it = CKeyOp::m_setKeyOp.begin(); it != CKeyOp::m_setKeyOp.end(); )
	{
		if (it->m_signal == m_keySignal)
		{
			CKeyOp::m_setKeyOp.erase(it++);
		}
		else {
			it++;
		}
	}
	while (!qKey.empty())
	{
		CKeyOp::m_setKeyOp.insert(qKey.front());

		qKey.pop();
	}
	::LeaveCriticalSection(&CKeyOp::g_csKeyOp);

	//delete all link actTemp's weight
	for (auto iter = g_weight.begin(); iter != g_weight.end(); )
	{
		if (iter->first.first == this || iter->first.second == this)
		{
			g_weight.erase(iter++);
		}
		else {
			iter++;
		}
	}

}

void ActTemp::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
}

//add all weight relative to the head
double Neural::sumUpRelativeWeight(void * head)
{
	double sum = 0;
	for (auto it = g_weight.begin(); it != g_weight.end(); it++)
	{
		if (it->first.first == head)
		{
			sum += ((Neural*)(it->first.second))->m_output *  it->second;
		}
	}
	return sum;
}

void Neural::makeWeight(void *point1, void *point2, double x)
{
	g_weight[make_pair(point1, point2)] = x;
	g_weight[make_pair(point2, point1)] = x;
}

void Neural::makeWeight(void * point1, void * point2, double p1Top2, double p2Top1)
{
	g_weight[make_pair(point1, point2)] = p1Top2;
	g_weight[make_pair(point2, point1)] = p2Top1;
}

void SelMonster::run()
{
	g_monNeural1 = NULL;
	g_monNeural2 = NULL;
	double maxMon1 = 0;
	double maxMon2 = 0;

	for (auto iter = g_AnyToMon[this].begin(); iter != g_AnyToMon[this].end(); iter++)
	{
		(*iter)->run();
	}
	for (size_t i = 0; i < 2; i++)
	{
		for (auto iter = g_AnyToMon[this].begin(); iter != g_AnyToMon[this].end(); iter++)
		{
			(*iter)->cal();
		}
	}


	for (auto iter = g_AnyToMon[this].begin(); iter != g_AnyToMon[this].end(); iter++)
	{
		auto itAct = g_weight.find(make_pair((void*)this, (void*)(*iter)));
		double curWeight = (*iter)->m_output;
		if (itAct != g_weight.end())
		{
			curWeight = (*iter)->m_output * itAct->second;
			//if (itAct->second <= 0)
			//{
			//	int a = 0;
			//}
		}
		if (curWeight > maxMon1)
		{
			maxMon1 = curWeight;
			g_monNeural1 = (*iter);
		}
		else if (curWeight > maxMon2)
		{
			maxMon2 = curWeight;
			g_monNeural2 = (*iter);
		}
	}
	//if (g_monNeural2 == NULL&& typeid( *g_monNeural1->getClassType()) ==  typeid(MonAttacking))
	//{
	//	int afds = 0;
	//}


}

void MonAny::run()
{
	m_Mon = g_RoomState.m_Monster;
	m_selfOutput = m_base + 3.33;
}

void MonAny::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
//	cout <<"monAny: "<< m_output<< " "<<m_Mon.m_vecCMon.size()<<"\t";
}

CRectangle AddR2ToR1(const CRectangle R1, const CRectangle R2)
{
	CRectangle r(R1.x - R2.width / 2, R1.y - R2.height / 2, R1.width + R2.width, R1.height + R2.height );
	return r;
}


void MonNearPlayer::run()
{
	auto &monster = g_RoomState.m_Monster;
	auto &player = g_RoomState.m_Player;
	m_Mon.m_vecCMon.clear();
	CRectangle curArea = AddR2ToR1(player.m_rect, m_nearArea);
	for (auto iter = monster.m_vecCMon.begin(); iter != monster.m_vecCMon.end() ; iter++)
	{
		if (CRectangle::RectCollide(iter->m_rect, curArea) == 1)
		{
			m_Mon.m_vecCMon.push_back(*iter);
		}
	}
	m_selfOutput = m_base;
	m_selfOutput += m_numToScore( m_Mon.m_vecCMon.size());

}

void MonNearPlayer::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
}

void MonAttacking::run()
{
	auto &attackArea = g_RoomState.m_AttackEffect;
	auto &monster = g_RoomState.m_Monster;
	DWORD nowTime = ::GetTickCount();
	m_Mon.m_vecCMon.clear();
	m_Mon.m_time = nowTime;
	m_selfOutput = m_base;

	for (auto iter = attackArea.rbegin(); iter != attackArea.rend(); iter++)
	{
		if (nowTime - iter->attackTime > 4 * 1000)
			break;
		for (auto itMon = monster.m_vecCMon.begin(); itMon != monster.m_vecCMon.end(); itMon++)
		{
			if (CRectangle::RectCollide(iter->attackRect, itMon->m_rect) == 1)
			{
				int ok = 1;
				for (auto itDuplicate = m_Mon.m_vecCMon.begin(); itDuplicate != m_Mon.m_vecCMon.end(); itDuplicate++)
				{
					if (itDuplicate->m_rect.compare(itMon->m_rect))
					{
						ok = 0;
						break;	
					}
				}
				if (ok == 1)
				{
					m_Mon.m_vecCMon.push_back(*itMon);
				}
			}
		}
	}
//	cout <<"attacking "<< m_Mon.m_vecCMon.size()<< "\t";

	m_selfOutput += m_numToScore(m_Mon.m_vecCMon.size());
}

void MonAttacking::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
}

void ActGoToMonsterOpposite::run()
{
	CRectangle detectArea(0, 0, 100, 60);
	int leftMonsterNumber = 0;
	int rightMonsterNumber = 0;

	int sideMonsterCount = de::TwoSideMonsterNumber(g_RoomState.m_Player.m_rect.midLine(), detectArea, g_RoomState.m_Monster, leftMonsterNumber, rightMonsterNumber);

	for (auto iter = g_RoomState.m_Monster.m_vecCMon.begin(); iter != g_RoomState.m_Monster.m_vecCMon.end(); iter++)
	{
		vector<CRectangle> receive;
		de::generateTwoSideArea(iter->m_rect, detectArea, receive);
		for (auto itVoidArea = receive.begin(); itVoidArea != receive.end(); itVoidArea++)
		{
			int leftCount = 0;
			int rightCount = 0;
			int minuMonsterCount =  de::TwoSideMonsterNumber(itVoidArea->midLine(), detectArea, g_RoomState.m_Monster, leftCount, rightCount);
			CAttackArea possibleArea;
			possibleArea.m_rect = *itVoidArea;
			possibleArea.score = 0 - minuMonsterCount*8;
			m_area.push_back(possibleArea);
		}
	}

	de::calZeroAttackArea(m_area, sideMonsterCount*14);
	de::calAttackAreaScoreInMove(m_area, g_RoomState.m_Player, 0, 0, 0.1, 0.12);

	m_bestArea = de::selBestAttackArea(m_area);
	m_selfOutput = m_base;
	m_selfOutput += m_bestArea.score;
}

void ActGoToMonsterOpposite::cal()
{
	m_output = m_selfOutput;
	m_output += Neural::sumUpRelativeWeight(this);
}

void ActGoToMonsterOpposite::express()
{
	ActTemp* actAttack = new ActTemp();
	DWORD nowTime = GetTickCount();
	actAttack->m_beginTime = nowTime;
	actAttack->creator = this;
	actAttack->m_base = 10;
	actAttack->m_fnOutput = ActTemp::fnOutMustRunComplete();
	CTrail tra;

	CRectangle::getRectTrail(g_RoomState.m_Player.m_rect, m_bestArea.m_rect, tra);
	actAttack->m_endTime = actAttack->m_beginTime + ga::timeActTempToStart;
	actAttack->m_trail.push_back(tra);
	g_AnyToActTemp[&g_action].insert(actAttack);
}
