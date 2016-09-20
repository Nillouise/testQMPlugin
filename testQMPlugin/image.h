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
		extern byte* g_pbCurScreen;
		extern CRectangle g_rect;
	}
	int getNewScreen(Cdmsoft dm, CRectangle screen = CRectangle(0, 0, 800, 600));
	class ColRGB
	{
	public:
		byte *col;
		enum RGBTYPE
		{
			R = 2,//use in ima namespace
			G = 1,//use in ima namespace
			B = 0,//use in ima namespace

		};
		ColRGB() { col = new byte[3]; oriPb = col; }
		ColRGB(byte* pbCol) { col = pbCol; oriPb = NULL; }
		ColRGB(byte Red, byte Green, byte Blue) { col = new byte[3]; oriPb = col; col[R] = Red; col[G] = Green; col[B] = Blue; }
		ColRGB(const ColRGB& C)
		{
			col = new byte[3]; oriPb = col;
			col[R] = C.col[R];
			col[G] = C.col[G];
			col[B] = C.col[B];
		}
		~ColRGB() { delete oriPb; }

		bool operator==(const ColRGB &t1) const {
			return col[R] == t1.col[R] && col[G] == t1.col[G] && col[B] == t1.col[B];
		}

	private:
		byte *oriPb;
	};

	class CBlock :public CRectangle
	{
	public:
		CBlock(int x = 0, int y = 0, int width = 0, int height = 0, const ColRGB pb = ColRGB()) :CRectangle(x, y, width, height),m_color(pb){}
		ColRGB m_color;
		static int getBlock(const vector<ColRGB> &color, set<CBlock> &receive);
		bool operator < (const CBlock &t1)const;
	};


	//template<class T>
	//bool convertRGB2BGR(vector<T*>& color)
	//{
	//	for (auto iter = color.begin(); iter != color.end(); iter++)
	//	{
	//		T temp = (*iter)[R];
	//		(*iter)[R] = (*iter)[B];
	//		(*iter)[B] = temp;
	//	}
	//	return true;
	//}
	inline byte getColorRGB(const int &x, const int &y, const ColRGB::RGBTYPE &color)
	{
		return *(curScreen::g_pbCurScreen + y * curScreen::g_rect.width + x * 4 + color);
	}
	inline byte* getColorWhole(const int &x, const int &y)
	{
		return curScreen::g_pbCurScreen + y * (curScreen::g_rect.width*4) + x * 4;
	}

	inline bool compareTwoColor(const byte *a,const byte *b)
	{
		if (*(a+ ColRGB::R) == *(b+ ColRGB::R) && *(a+ ColRGB::G) == *(b+ ColRGB::G) && *(a+ ColRGB::B) == *(b+ ColRGB::B))
		{
			return true;
		}
		return false;
	}
	inline bool BigEatSmall(const CBlock &eater, const CBlock &food)
	{
		if (compareTwoColor(eater.m_color.col, food.m_color.col))
		{
			if (eater.x <= food.x && eater.y <= food.y && eater.x + eater.width >= food.x + food.width && eater.y + eater.height <= food.y + food.height)
				return true;
			else
				return false;
		}
		else
			return false;
	}

}