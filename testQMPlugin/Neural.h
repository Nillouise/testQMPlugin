#pragma once
#include"stdafx.h"
#include"Room.h"
#include<iostream>
#include<vector>
#include<map>
#include<set>
using namespace std;
using namespace gandalfr;





class Neural
{
public:

	virtual void run() {}
	virtual void link(){}
	virtual void express() {}
	virtual void end() {}
	virtual Neural* getClassType(){ return this; }
	double m_output;
	double m_base;

	DWORD m_lastRunTime;
	DWORD m_lastExpressTime;
	DWORD m_runInterval;

};


class MonNeural:public Neural
{
public:
	virtual MonNeural* getClassType() { return this; }
	CMonsterSet m_Mon;
	
};

class MonNearAndAttacking :public MonNeural
{
public:
	virtual MonNearAndAttacking* getClassType() { return this; }
	virtual void run();
};



class ActNeural :public Neural
{
public:
	virtual ActNeural* getClassType() { return this; }
	DWORD m_lastReleaseSkills;

	MonNeural **m_MonToAttack;// point to a fix variant that point to dynamic MonNeural

};


class ActTemp :public Neural
{
public:
	DWORD beginTime;
	DWORD endTime;
	vector<CKeyOp>m_key;
	vector<CTrail> m_trail;
	double(*m_fnOutput)(DWORD begin, DWORD end);
	ActNeural* creator;//who create it

	ActTemp(CTrail m_trail, vector<CKeyOp>m_key, DWORD beginTime, DWORD endTime) {};
	ActTemp() {};
	virtual ActTemp* getClassType() { return this; }
	virtual void run();
	virtual void express();
	virtual void end();
};


class CAction
{
public:
	Neural* m_curAct;

	vector<ActNeural*> m_hisActNeural;

	void run();
	static DWORD executeTrail( const vector<CTrail>& trail);// it must be changed to vector<CTrail>

	static DWORD playerRunX(int x, map<wstring, int> &runState, const CSpeed &speed, const DWORD &beginTime, int runOrWalk);

	static DWORD playerRunY(int y, map<wstring, int> &runState, const CSpeed &speed, const DWORD &beginTime);


};



template<typename T>
struct comp
{
	bool operator ()(const T *a, const T *b)
	{
		return (void*)a < (void*)b;
	}
};

extern map<pair<void*, void*>, double> g_weight;
extern map<void*, set<ActNeural*, comp<ActNeural>> >  g_AnyToAct;
extern map<void*, set<ActTemp*, comp<ActTemp>> >  g_AnyToActTemp;
extern map<void*, set<MonNeural*, comp<MonNeural>> >  g_AnyToMon;
extern CAction g_action;