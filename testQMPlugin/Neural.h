#pragma once
#include"stdafx.h"
#include"Room.h"
#include<iostream>
#include<vector>
#include<map>
#include<set>
using namespace std;
using namespace gandalfr;

extern map<pair<void*, void*>, double> g_weight;
extern map<void*, set<ActNeural*, comp<ActNeural>> >  g_AnyToAct;
extern map<void*, set<ActTemp*, comp<ActTemp>> >  g_AnyToActTemp;

extern CAction g_action;



class Neural
{
public:

	virtual void run() {}
	virtual void link(){}
	virtual void express() {}
	double m_output;
	DWORD m_lastRunTime;
	double m_base;

};


class ActNeural :public Neural
{
public:
	CTrail m_trail;
	std::vector<CKeyOp> m_key;
	int newExpress;//if it need to express one more;
	virtual void express();

};


class ActTemp :public ActNeural
{
	
	virtual void express();


};


class CAction
{
public:
	ActNeural* m_curAct;

	void run();
	static DWORD keyboard( const CTrail& trail);

	static DWORD playerRunX(int x);

	static DWORD playerRunY(int y);


};





template<typename T>
struct comp
{
	bool operator ()(const T *a, const T *b)
	{
		return (void*)a < (void*)b;
	}
};