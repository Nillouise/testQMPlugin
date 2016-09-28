#pragma once
#include"stdafx.h"
#include<opencv2\opencv.hpp>
#include<iostream>
#include<sstream>
#include <iomanip>
#define VISUALIZATION
using namespace std;

namespace vis
{
	unsigned int __stdcall showView(PVOID pM);
	unsigned int __stdcall procView(PVOID pM);
	int init();

	inline string doubleToString(double x)
	{
		ostringstream oss;
		if (x < -20)
		{
			x = -20;
		}
		oss << setprecision(2) << x ;
		return oss.str();
	}


	extern CRITICAL_SECTION cs_visualization;

}