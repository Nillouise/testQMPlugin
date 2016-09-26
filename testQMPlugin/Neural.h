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
	virtual Neural* getClassType() { return this; }
	virtual void run() {}
	virtual void cal() {}
	virtual void express() {}
	virtual void end() {}

	//sum up the neural relative weigh in map g_weight
	static double sumUpRelativeWeight(void* head);
	//link point1 and point2 in g_weight
	static void makeWeight(void *point1, void *point2, double x);
	static void makeWeight(void *point1, void*point2, double p1Top2, double p2Top1);


	double m_selfOutput;//the run calculate this output;
	double m_output;// the Neural weight that involve all include m_base ,m_selfOutput and other link
	double m_base;//use to make the m_ouput

	DWORD m_lastRunTime;
	DWORD m_lastExpressTime;
	DWORD m_runInterval;//the time interval between two actual run

};


class MonNeural:public Neural
{
public:
	virtual MonNeural* getClassType() { return this; }
	CMonsterSet m_Mon;//this MonNeural store which monsters should be considerate
	
};


class MonNearAndAttacking :public MonNeural
{
public:
	virtual MonNearAndAttacking* getClassType() { return this; }
	virtual void run();
	virtual void cal();
};

class MonAny :public MonNeural
{
public:
	virtual MonAny* getClassType() { return this; }
	virtual void run();
	virtual void cal();
	MonAny(double base = 0.0) { m_base = base; };
};


//it didn't have outpu
class SelMonster
{
public:
	void run();

};


class ActNeural :public Neural
{
public:
	virtual ActNeural* getClassType() { return this; }
	DWORD m_lastReleaseSkills;// the time that last release Skill

	MonNeural **m_MonToAttack;// point to a fix variant that point to dynamic MonNeural

};

//it must have correspond Up Key if you use m_key,
class ActTemp :public Neural
{
public:
	virtual ActTemp* getClassType() { return this; }
	ActTemp(ActNeural* creator = NULL, double(*fnOutput)(DWORD begin, DWORD end, Neural*neural) = NULL) :creator(creator), m_fnOutput(fnOutput) { m_beginTime = GetTickCount(); m_endTime = GetTickCount() + 250; m_output = 0; m_base = 0; };

	static double fnOutMustRunComplete(DWORD beginTime, DWORD endTime, Neural* TempNeural);//tempNeural have creator

	virtual void run();//ActTemp output only depend on m_fnOutput?didn't relative the other Neural?
	virtual void express();
	virtual void end();
	virtual void cal();

	DWORD m_beginTime;
	DWORD m_endTime;

	vector<CTrail> m_trail;//if it is no empty,than didn't run m_key,else,run m_key
	vector<CKeyOp>m_key;
	int m_keySignal;//it only use in m_key

	double(*m_fnOutput)(DWORD begin, DWORD end,Neural* neural);//the function poiter to cal the ActTemp output should be what in this time
	ActNeural* creator;//who create it
};


class CAction
{
public:
	Neural* m_curActNeural;
	vector<pair<ActNeural*,DWORD> > m_hisActNeural;//record history express ActNeural and the begin time

	CAction() { m_curActNeural = NULL;}

	void run();

	static DWORD executeTrail(const vector<CTrail>& trail, const CSpeed & speed, const map<wstring, int>& runState);
	static DWORD playerRunX(int x, map<wstring, int> &runState, const CSpeed &speed, const DWORD &beginTime, int runOrWalk, DWORD &timeToYexcute);
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

extern map<pair<void*, void*>, double> g_weight;//the inner have correctly order
extern map<void*, set<ActNeural*, comp<ActNeural>> >  g_AnyToAct;
extern map<void*, set<ActTemp*, comp<ActTemp>> >  g_AnyToActTemp;//int this version code,ActTemp only have one in one time
extern map<void*, set<MonNeural*, comp<MonNeural>> >  g_AnyToMon;
extern CAction g_action;

extern SelMonster g_selMonster;
extern MonNeural* g_monNeural1;
extern MonNeural* g_monNeural2;