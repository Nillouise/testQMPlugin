#pragma once
#include"stdafx.h"
#include"Cdmsoft.h"
#include<vector>
#include<string>
#include<iostream>
#include"Room.h"
using namespace std;
using namespace gandalfr;
namespace ima
{
	namespace curScreen
	{
		byte* g_pbCurScreen=NULL;
		CRectangle g_rect;
	}
	enum RGBTYPE
	{
		R = 2,
		G = 1,
		B = 0,
	};
	class CBlock :public CRectangle
	{
	public:
		CBlock(int x = 0, int y = 0, int width = 0, int height = 0,const byte* pb = NULL) :CRectangle(x, y, width, height) { m_color[0] = *pb; m_color[1] = *(pb + 1); m_color[2] = *(pb + 2); }
		byte m_color[3];
		static int getBlock(const vector<byte[3]> &color, set<CBlock> &receive);
	};

	int getNewScreen(Cdmsoft dm, CRectangle screen = CRectangle(0, 0, 800, 600));

	template<class T>
	bool convertRGB2BGR(vector<T[3]>& color)
	{
		for (auto iter = color.begin(); iter != color.end(); iter++)
		{
			T temp = (*iter)[R];
			(*iter)[R] = (*iter)[B];
			(*iter)[B] = temp;
		}
		return true;
	}
	inline byte getColorRGB(const int &x, const int &y, const RGBTYPE &color)
	{
		return *(curScreen::g_pbCurScreen + y * curScreen::g_rect.width + x * 4 + color);
	}
	inline byte* getColorWhole(const int &x, const int &y)
	{
		return curScreen::g_pbCurScreen + y * curScreen::g_rect.width + x * 4;
	}

	inline bool compareTwoColor(const byte a[3],const byte b[3])
	{
		if (a[R] == b[R] && a[G] == b[G] && a[B] == b[B])
		{
			return true;
		}
		return false;
	}
}