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
//#define FROWE(x,width) for(int x = 0;x < width*4;x+=4)
//#define FCOLE(y,height) for(int y = 0;y < height;y++)
//#define FROW(x) for(int x = 0; x < curScreen::g_rect.width*4; x+=4)
//#define FCOL(y) for(int y = 0; y < curScreen::g_rect.height; y++)
//#define FROWS(x,step) for(int x = 0;x < curScreen::g_rect.width*4;x = x + 4*step)
//#define FCOLS(y,step) for(int y = 0;y < curScreen::g_rect.height;y += step)
//#define FROWB(x,beginx) for(int x = beginx*4; x < curScreen::g_rect.width*4; x+=4)
//#define FCOLB(y,beginy) for(int y = beginy; y < curScreen::g_rect.height; y++)
//#define FROWBD(x,beginx,decrease) for(int x = beginx*4; x < curScreen::g_rect.width*4; x=x + decrease*4)
//#define FCOLBD(y,beginy,decrease) for(int y = beginy; y < curScreen::g_rect.height; y=y+decrease)
//

	int ima::getNewScreen(Cdmsoft dm, CRectangle screen )
	{
		curScreen::g_pbCurScreen = (byte*)dm.GetScreenData(screen.x, screen.y, screen.x +screen.width, screen.y + screen.height);
		curScreen::g_rect = screen;
		return 0;
	}

	//may be in other computer ,the memory layout is different
	//return set<Cblock> receive is  relative coordinate to the curScreen::g_rect,and it's up-left point is out of the color rect a point;
	int ima::CBlock::getBlock(const vector<ColRGB> &sourecolor, set<CBlock>& receive)
	{
		auto &pbyte = curScreen::g_pbCurScreen;
		auto &rect = curScreen::g_rect;
		//because the memory layout is bgr,it must convert rgb to bgr(may be in other computer ,layout is unanimous;
		//now it is fix,because,the out of ima namespace will use a correct enum type to regonize memory


		set<CBlock> griddle;
		int stepX = 10;
		int stepY = 10;
		for(int y = 0;y < rect.height;y += stepY)
			for(int x = 0;x<rect.width;x += stepX)
			{
				for (auto it = sourecolor.begin(); it != sourecolor.end(); it++)
				{
					if (compareTwoColor(it->col, getColorWhole(x, y)))
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
			auto &griRGB = iter->m_color.col;
			int upY = 0;
			int downY = 0;
			int leftX = 0;
			int rightX = 0;
			int x;
			for (x = griX; x >= rect.x; x--)
			{
				if (x == -1 || leftX == -1)
				{
					int dffd = 0;
				}
				if (compareTwoColor(getColorWhole(x, griY), griRGB) == false)
					break;
			}
			leftX = x;

			for (x = griX; x <= rect.x + rect.width; x++)
			{
				if (compareTwoColor(getColorWhole(x, griY), griRGB) == false)
					break;
			}
			rightX = x;


			int y;
			for (y = griY; y >= rect.y; y--)
			{
				if (compareTwoColor(getColorWhole(griX, y), griRGB) == false)
					break;
			}
			upY = y;
			for (y = griY; y <= rect.y + rect.height; y++)
			{
				if (compareTwoColor(getColorWhole(griX, y), griRGB) == false)
					break;
			}
			downY = y;
			countBlockAppear[CBlock(leftX, upY, rightX - leftX, downY - upY, griRGB)]++;
		}
		for (auto iter = countBlockAppear.begin(); iter != countBlockAppear.end(); )
		{
			if (iter->second < (iter->first.width / stepX) * (iter->first.height / stepY))
			{
				iter = countBlockAppear.erase(iter);
			}
			else
			{
				iter++;
			}
				
		}
		for (auto iter = countBlockAppear.begin(); iter != countBlockAppear.end();iter++ )
		{
			receive.insert(iter->first);
		}

		return 0;
	}

	bool CBlock::operator<(const CBlock & t1) const
	{
		if (x < t1.x)
			return true;
		else if (x > t1.x)
			return false;

		if (y < t1.y)
			return true;
		else if (y > t1.y)
			return false;

		if (width < t1.width)
			return true;
		else if (width > t1.width)
			return false;

		if (height < t1.height)
			return true;
		else if (height > t1.height)
			return false;

		if (m_color.col[ColRGB::R] < t1.m_color.col[ColRGB::R])
			return true;
		else if (m_color.col[ColRGB::R] > t1.m_color.col[ColRGB::R])
			return false;
		if (m_color.col[ColRGB::G] < t1.m_color.col[ColRGB::G])
			return true;
		else if (m_color.col[ColRGB::G] > t1.m_color.col[ColRGB::G])
			return false;
		if (m_color.col[ColRGB::B] < t1.m_color.col[ColRGB::B])
			return true;
		else if (m_color.col[ColRGB::B] > t1.m_color.col[ColRGB::B])
			return false;
		return false;
	}

}