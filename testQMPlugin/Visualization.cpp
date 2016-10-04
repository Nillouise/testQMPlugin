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
	Mat DnfRoomState(Mat &image,CRectangle &screen,int narrowRate = 2)
	{
		int dnfWidth = 800;
		int dnfHeight = 600;
		int dnfedgeWidth = 30;

		int dnfScreenWidth = dnfWidth / narrowRate + 2 * dnfedgeWidth;
		int dnfScreenHeight = dnfHeight / narrowRate + 2 * dnfedgeWidth;

		image.create(dnfScreenHeight, dnfScreenWidth, CV_8UC3);
		rectangle(image, Point(0, 0),Point(image.cols,image.rows ), Scalar(0, 0, 0), CV_FILLED, 8);
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

	CRectangle MonNeuralState(Mat&image,const CRectangle &screen)
	{
		if (g_monNeural1 != NULL)
		{
			string neuralName = typeid((*g_monNeural1->getClassType())).name();
			neuralName = neuralName.substr(6, neuralName.size() - 6);
			string onemsg = neuralName + " " + doubleToString(g_monNeural1->m_output) + " MonNumber:"+ doubleToString( g_monNeural1->m_Mon.m_vecCMon.size());
			putText(image, onemsg, Point(screen.x, screen.y + 12), FONT_HERSHEY_PLAIN, 1, mapColor[player], 1, 8);
		}
		else {
			putText(image, "g_monNeural1: NULL", Point(screen.x, screen.y + 12), FONT_HERSHEY_PLAIN, 1, mapColor[player], 1, 8);
		}
		if (g_monNeural2 != NULL)
		{

			string neuralName1 = typeid((*g_monNeural2->getClassType())).name();
			neuralName1 = neuralName1.substr(6, neuralName1.size() - 6);
			string onemsg1 = neuralName1 + " " + doubleToString(g_monNeural2->m_output)+" MonNumber:" + doubleToString(g_monNeural2->m_Mon.m_vecCMon.size());
			putText(image, onemsg1, Point(screen.x, screen.y + 24), FONT_HERSHEY_PLAIN, 1, mapColor[player], 1, 8);
		}
		else {
			putText(image, "g_monNeural2: NULL", Point(screen.x, screen.y + 24), FONT_HERSHEY_PLAIN, 1, mapColor[player], 1, 8);
		}
		CRectangle r(screen);
		r.y += 24;
		return r;
	}


	Mat ActionNeuralState(Mat &image)
	{
		const auto &action = g_AnyToAct[&g_action];
		int width = 800;
		int height = 500;
		image.create(height, width, CV_8UC3);
		rectangle(image, Point(0, 0), Point(image.cols, image.rows), Scalar(0, 0, 0), CV_FILLED, 8);
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

	std::string ws2s(const std::wstring& ws)
	{
		std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
		setlocale(LC_ALL, "chs");
		const wchar_t* _Source = ws.c_str();
		size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest, _Source, _Dsize);
		std::string result = _Dest;
		delete[]_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	Mat printRunState(Mat &image,CRectangle printArea)
	{
		int startX = printArea.x;
		int startY = printArea.y;
		int OneHeight = 12;
		for (auto iter = g_RoomState.m_runState.begin(); iter != g_RoomState.m_runState.end(); iter++)
		{
			if (iter->second == 0)
				continue;
			string run = ws2s(iter->first);
			run += " ";
			run += doubleToString( iter->second);
			putText(image, run, Point(startX, startY + OneHeight), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1, 8);
			startY += OneHeight;
		}
		return image;
	}

	int printPlayerDirection(Mat&image, CRectangle printArea)
	{
		string msg = "player:Direction: ";
		msg+=doubleToString(g_RoomState.m_Player.m_direction);
		int OneHeight = 12;
		putText(image, msg, Point(printArea.x, printArea.y + OneHeight), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1, 8);
		return 0;
	}



	Mat printKeyboardState(Mat &image, CRectangle printArea)
	{
		int startX = printArea.x;
		int startY = printArea.y;
		int OneHeight = 12;
		for (auto iter = CKeyOp::m_keyStateSignal.begin(); iter != CKeyOp::m_keyStateSignal.end(); iter++)
		{
			if (iter->second == 1 || iter->second == 0)
				continue;
			string run = ws2s(iter->first);
			run += " ";
			run += doubleToString(iter->second);
			putText(image, run, Point(startX, startY + OneHeight), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 1, 8);
			startY += OneHeight;
		}
		return image;
	}


	Mat keyBoardState(Mat &image)
	{
		int width = 400;
		int height = 300;
		image.create(height, width, CV_8UC3);
		rectangle(image, Point(0, 0), Point(image.cols, image.rows), Scalar(0, 0, 0), CV_FILLED, 8);
		printRunState(image, CRectangle(0, 0, 200, 40));
		printKeyboardState(image, CRectangle(0, 40, 200, 40));
		printPlayerDirection(image, CRectangle(0, 80 , 200, 24));
		MonNeuralState(image, CRectangle(0, 110, 200, 24));

		return image;
	}




	Mat dnfMat(10, 10, CV_8UC3);
	Mat neuMat(10, 10, CV_8UC3);
	Mat keyboardMat(10, 10, CV_8UC3);
	int procedMat = 1;
	unsigned int __stdcall showView(PVOID pM)
	{
		while (controlVisThread>0)
		{
			if (controlVisThread == 2)
				continue;
			if (procedMat == 1)
			{
				imshow("RoomState", dnfMat);
				imshow("NeuralState", neuMat);
				imshow("keyBoardState", keyboardMat);
				procedMat = 0;
			}
			int r=-1;

			r = waitKey(33);
			if (r != -1)
			{
				controlVisThread = 0;
				destroyWindow("dnfScreen");
			}
		}
		return 0;
	}
	unsigned int __stdcall procView(PVOID pM)
	{

		int headHeight = 100;
		controlVisThread = 1;

		while (controlVisThread>0)
		{
			if (controlVisThread == 2)
				continue;
			::EnterCriticalSection(&cs_visualization);
			CRectangle screen;
			DnfRoomState(dnfMat,screen);
			printBestArea(dnfMat, screen);
			ActionNeuralState(neuMat);

			keyBoardState(keyboardMat);

			::LeaveCriticalSection(&cs_visualization);
			procedMat = 1;
		}

		return 0;
	}




}