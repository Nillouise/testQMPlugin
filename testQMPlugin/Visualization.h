#pragma once
#include"stdafx.h"
#include<opencv2\opencv.hpp>
#define VISUALIZATION

namespace vis
{
	unsigned int __stdcall showView(PVOID pM);
	int init();

	extern CRITICAL_SECTION cs_visualization;

}