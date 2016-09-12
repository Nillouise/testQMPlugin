#include<iostream>
#include<vector>
#include<map>
#include<set>
#include<Windows.h>
using namespace std;

map<pair<void*, void*>, double> map_weight;

class CRect
{
public:
	int x;
	int y;
};

class target
{
public:
	virtual void run() {};
	CRect pos;

};

class destination :public target
{
public:
	static destination* curObject;
	virtual void run() 
	{
		pos.x = 100;
		pos.y = 100;
	}
	static destination* getClass()
	{
		if (curObject == NULL)
		{
			curObject = new destination();
			cout << "create destination" << endl;
		}


		return curObject;
	}


};
 destination* destination::curObject;

class destinationWithObstacle :public target
{
public:
	virtual void run()
	{
		pos.x = 200;
		pos.y = 200;

	}
	static destinationWithObstacle* curObject;
	static destinationWithObstacle* getClass()
	{
		if (curObject == NULL)
		{
			curObject = new destinationWithObstacle();
			cout << "create destinationWithObstacle" << endl;
		}
		return curObject;
	}
};
destinationWithObstacle* destinationWithObstacle::curObject;


template<typename T>
struct comp
{
	bool operator ()(const T *a, const T *b)
	{
		return (void*)a < (void*)b;
	}
};


class targetTarget
{
public:
	set<target*,comp<target> > set_Uplevel;

	static targetTarget* curObject;
	static targetTarget* getClass()
	{
		if (curObject == NULL)
		{
			curObject = new targetTarget();
			cout << "create targetTarget" << endl;
		}


		return curObject;
	}

};

targetTarget* targetTarget::curObject;


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
		curObject = this;
		targetTarget *a = targetTarget::getClass();
		a->set_Uplevel.insert( (target*) destination::getClass());
		a->set_Uplevel.insert((target*)destinationWithObstacle::getClass());
	}

};
player* player::curObject;


int main()
{
	cout << (void*)player::getClass() << endl;
	cout << (void*)player::getClass()->curObject << endl;
	(*targetTarget::getClass()->set_Uplevel.begin())->run();
	cout << (*targetTarget::getClass()->set_Uplevel.begin())->pos.x << endl;
	auto it = targetTarget::getClass()->set_Uplevel.begin();
	it++;
	(*it)->run();
	auto it2 = *it;
	cout << (*it)->pos.x << endl;
	cout << (*(--it))->pos.x << endl;
	map_weight[make_pair((void *)*it,(void*)*it)]  = 2500.0;
	cout << map_weight[make_pair((void *)*it, (void*)*it)];




	getchar();
	return 0;
}










