#include"stdafx.h"
#include<iostream>
#include<vector>
#include<map>
#include<set>
using namespace std;

map<pair<void*, void*>, double> map_weight;

class player
{
public:
	CRect pos;
	double output;
	static player* curObject;
	static player* getClass()
	{
		if (curObject == NULL)
		{
			curObject = new player();
		}
		return curObject;
	}

	player()
	{

	}

};


class target
{

};

class destination :public target
{


};

class destinationWithObstacle:public target
{

};

class targetTarget
{
public:
	set<target> set_Uplevel;

	static targetTarget* curObject;
	static targetTarget* getClass()
	{
		if (curObject == NULL)
		{
			curObject = new targetTarget();
		}
		return curObject;
	}



};

template<typename T>

struct comp
{
	bool operator ()(const T &a, const T &b)
	{
		return a>b;
	}
};