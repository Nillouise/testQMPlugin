#include"stdafx.h"
#include"Visualization.h"
#include<string>
#include<map>
#include"Room.h"
#include"Neural.h"
#include <sstream>
using namespace cv;
using namespace std;
using namespace gandalfr;
namespace vis
{
	CRITICAL_SECTION cs_visualization;
	map<string, Scalar> mapColor;
	const string monster = "monster";
	const string obstacle = "obstacle";
	const string gold = "gold";
	const string scenebox = "scenebox";
	const string player = "player";
	const string text = "text";
	int controlVisThread = 1;//1 is to run,2 is to stop,0 is to exit


	int init()
	{
		mapColor[monster] = Scalar(0xff, 0x00, 0x94);
		mapColor[player] = Scalar(255, 255, 255);
		mapColor[text] = Scalar(255, 255, 255);
		InitializeCriticalSection(&cs_visualization);
		return 0;
	}

	//if rect is out of edge,it print beside to the edge
	int printRect(Mat &canvas,const CRectangle &rect,Scalar &color,const CRectangle &edge,int fill = CV_FILLED)
	{
		CRectangle recPro(rect);
		if (recPro.x + recPro.width < 0)
			recPro.x = - recPro.width;
		if (recPro.x > edge.width)
			recPro.x = edge.width;
		if (recPro.y + recPro.height < 0)
			recPro.y = - recPro.height;
		if (recPro.y > edge.height)
			recPro.y = edge.height;

		Point p1(recPro.x + edge.x, recPro.y + edge.y);
		Point p2(recPro.width + recPro.x + edge.x - 1, recPro.height+ recPro.y + edge.y-1);
		rectangle(canvas,p1,p2,color, fill,8);
		return 0;
	}
	struct cmp_Neural
	{
		bool operator()(void * k1, void * k2)const
		{
			if (((Neural*)k1)->m_output > ((Neural*)k2)->m_output)
				return true;
			else if (((Neural*)k1)->m_output < ((Neural*)k2)->m_output)
				return false;
			if (k1 > k2)
				return true;
			else
				return false;
		}
	};
	Mat DnfRoomState(CRectangle &screen,int narrowRate = 2)
	{
		int dnfWidth = 800;
		int dnfHeight = 600;
		int dnfedgeWidth = 30;

		int dnfScreenWidth = dnfWidth / narrowRate + 2 * dnfedgeWidth;
		int dnfScreenHeight = dnfHeight / narrowRate + 2 * dnfedgeWidth;

		Mat image = Mat::zeros(dnfScreenHeight, dnfScreenWidth, CV_8UC3);

		screen.x = dnfedgeWidth;
		screen.y = dnfedgeWidth;
		screen.width = dnfScreenWidth - 2 * dnfedgeWidth;
		screen.height = dnfScreenHeight - 2 * dnfedgeWidth;


		if (g_RoomState.m_Player.m_rect.x > 0)
		{
			auto newRect(g_RoomState.m_Player.m_rect);
			newRect.x /= 2;
			newRect.y /= 2;
			newRect.width /= 2;
			newRect.height /= 2;
			printRect(image, newRect, mapColor[player], screen);
		}
		else
		{
			putText(image, "No player!", Point(0, dnfedgeWidth / 2), FONT_HERSHEY_PLAIN, 1,mapColor[player], 1, 8);
		}

		for (auto iter = g_RoomState.m_Monster.m_vecCMon.begin() ; iter != g_RoomState.m_Monster.m_vecCMon.end(); iter++)
		{
			auto newRect(iter->m_rect);
			newRect.x /= 2;
			newRect.y /= 2;
			newRect.width /= 2;
			newRect.height /= 2;
			printRect(image, newRect, mapColor[monster], screen);
		}
		return image;
	}



	Mat ActionNeuralState()
	{
		const auto &action = g_AnyToAct[&g_action];
		int width = 400;
		int height = 300;
		Mat image = Mat::zeros(height, width, CV_8UC3);
		static map<void*, CRectangle, cmp_Neural> neuralToRect;// big to small
		neuralToRect.clear();
		for (auto iter = action.begin(); iter != action.end(); iter++)
		{
			neuralToRect[*iter];
		}
		for (auto iter = g_AnyToActTemp[&g_action].begin(); iter != g_AnyToActTemp[&g_action].end(); iter++)
		{
			neuralToRect[*iter];
		}
		int OneWidth = 250;
		int OneHeight = 12;

		int startX = 0;
		int startY = 0;

		if (g_action.m_curActNeural != NULL)
		{
			string curNeural = typeid(*g_action.m_curActNeural->getClassType()).name();
			curNeural += " " + doubleToString(g_action.m_curActNeural->m_output);
			putText(image, curNeural, Point(startX, startY + OneHeight), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 1, 8);
		}
		else
		{
			putText(image, "NULL", Point(startX, startY + OneHeight), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255), 1, 8);
		}
		startY += OneHeight;
		for (auto iter = neuralToRect.begin() ; iter != neuralToRect.end(); iter++)
		{
			CRectangle One(startX, startY, OneWidth, OneHeight);
			startY += OneHeight;
			if (startY > height - OneHeight)
			{
				startY = 0;
				startX += OneWidth;
			}
			(*iter).second = One;

		}
		for (auto iter = neuralToRect.begin(); iter != neuralToRect.end(); iter++)
		{
			((Neural*)(iter->first))->m_output;

			Neural *const neu = ((Neural*)(iter->first));

			string neuralName = typeid((*neu->getClassType())).name();
			neuralName = neuralName.substr(6, neuralName.size() - 6);
			string onemsg = neuralName + " " +  doubleToString(neu->m_output) ;
			putText(image, onemsg, Point(iter->second.x, iter->second.y+OneHeight), FONT_HERSHEY_PLAIN, 1, mapColor[player], 1, 8);
		}


//		Mat image = Mat::zeros(dnfHeight / narrowRate + 2 * dnfedgeWidth, dnfWidth / narrowRate, CV_8UC3);

		return image;

	}

	int shrinkRect(CRectangle& rect, int rate=2)
	{
		rect.x /= rate;
		rect.y /= rate;
		rect.width /= rate;
		rect.height /= rate;
		return 0;
	}

	int printBestArea(Mat &image, const CRectangle &edge)
	{
		if (g_action.m_hisActNeural.size() == 0)
		{
			return 0;
		}
		auto neu = g_action.m_hisActNeural[g_action.m_hisActNeural.size() - 1].first;
		auto name = neu->getBaseType();
		if (neu->getBaseType() != "ActWithArea")
			return 0;
		ActWithArea* actArea = (ActWithArea*)neu;

		Scalar colArea(0, 100, 255);
		CRectangle rect ( actArea->m_bestArea.m_rect);
		rect.x -= 2;
		rect.y -= 2;
		rect.width += 4;
		rect.height += 4;
		shrinkRect(rect);
		printRect(image, rect, colArea, edge, 2);
		return 0;

	}

	unsigned int __stdcall showView(PVOID pM)
	{

		int headHeight = 100;
		controlVisThread = 1;


		while (controlVisThread>0)
		{
			if (controlVisThread == 2)
				continue;
			::EnterCriticalSection(&cs_visualization);
			CRectangle screen;
			Mat roomstate = DnfRoomState(screen);
			printBestArea(roomstate, screen);
			imshow("dnfScreen", roomstate);
			imshow("Neural", ActionNeuralState());


			::LeaveCriticalSection(&cs_visualization);
			int r = waitKey(100);
			if (r != -1)
			{
				controlVisThread = 0;
				destroyWindow("dnfScreen");
			}
		}

		return 0;
	}




}