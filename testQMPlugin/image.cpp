#include"stdafx.h"
#include "image.h"
#include <iomanip>
using namespace std;
namespace ima
{
	namespace curScreen
	{
		byte* g_pbCurScreen = NULL;
		CRectangle g_rect;
	}
#define FROWE(x,width) for(int x = 0;x < width*4;x+=4)
#define FCOLE(y,height) for(int y = 0;y < height;y++)
#define FROW(x) for(int x = 0; x < curScreen::g_rect.width*4; x+=4)
#define FCOL(y) for(int y = 0; y < curScreen::g_rect.height; y++)
#define FROWS(x,step) for(int x = 0;x < curScreen::g_rect.width*4;x = x + 4*step)
#define FCOLS(y,step) for(int y = 0;y < curScreen::g_rect.height;y += step)
#define FROWB(x,beginx) for(int x = beginx*4; x < curScreen::g_rect.width*4; x+=4)
#define FCOLB(y,beginy) for(int y = beginy; y < curScreen::g_rect.height; y++)
#define FROWBD(x,beginx,decrease) for(int x = beginx*4; x < curScreen::g_rect.width*4; x=x + decrease*4)
#define FCOLBD(y,beginy,decrease) for(int y = beginy; y < curScreen::g_rect.height; y=y+decrease)


	int ima::getNewScreen(Cdmsoft dm, CRectangle screen )
	{
		curScreen::g_pbCurScreen = (byte*)dm.GetScreenData(screen.x, screen.y, screen.width, screen.height);
		curScreen::g_rect = screen;
		return 0;
	}

	//may be in other computer ,the memory layout is different
	int ima::CBlock::getBlock(const vector<byte*> &sourecolor, set<CBlock>& receive)
	{
		auto &pbyte = curScreen::g_pbCurScreen;
		auto &rect = curScreen::g_rect;
		auto color(sourecolor);
		//because the memory layout is bgr,it must convert rgb to bgr(may be in other computer ,layout is unanimous;
		for (auto it = color.begin(); it != color.end(); it++)
		{
			auto temp = (*it)[R];
			(*it)[R] = (*it)[B];
			(*it)[B] = temp;
		}


		set<CBlock> griddle;
		int stepX = 10;
		int stepY = 10;
		FROWS(x, stepX)
			FCOLS(y, stepY)
		{
			for (auto it = color.begin(); it != color.end(); it++)
			{
				if (compareTwoColor(*it, getColorWhole(x, y)))
				{
					CBlock toBlo(x, y, 0, 0, *it);
					griddle.insert(toBlo);
				}
			}
		}

		map<CBlock,int> countBlockAppear;
		for (auto iter = griddle.begin(); iter != griddle.end(); iter++)
		{
			const int &griX = iter->x;
			const int &griY = iter->y;
			auto &griRGB = iter->m_color;
			int upY = 0;
			int downY = 0;
			int leftX = 0;
			int rightX = 0;
			int x;
			for (x = griX; x >= curScreen::g_rect.x; x--)
			{
				if (compareTwoColor(getColorWhole(x, griY), griRGB) == false)
					break;
			}
			leftX = x;
			for (x = griX; x <= curScreen::g_rect.x + curScreen::g_rect.width; x++)
			{
				if (compareTwoColor(getColorWhole(x, griY), griRGB) == false)
					break;
			}
			rightX = x;
			int y;
			for (y = griY; y >= curScreen::g_rect.y; y--)
			{
				if (compareTwoColor(getColorWhole(griX, y), griRGB) == false)
					break;
			}
			upY = y;
			for (y = griY; y >= curScreen::g_rect.y + curScreen::g_rect.height; y++)
			{
				if (compareTwoColor(getColorWhole(griX, y), griRGB) == false)
					break;
			}
			downY = y;
			countBlockAppear[CBlock(leftX, upY, rightX - leftX, downY - upY, griRGB)]++;
		}
		for (auto iter = countBlockAppear.begin(); iter != countBlockAppear.end(); )
		{
			if (iter->second < (iter->first.width / stepX) * (iter->first.height /stepY))
				iter = countBlockAppear.erase(iter);
			else
				iter++;
		}
		for (auto iter = countBlockAppear.begin(); iter != countBlockAppear.end(); )
		{
			receive.insert(iter->first);
		}

		return 0;
	}
}