#include"stdafx.h"
#include"Visualization.h"
#include<string>
#include<map>
#include"Room.h"
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

	int controlVisThread = 1;//1 is to run,2 is to stop,0 is to exit


	int init()
	{
		mapColor[monster] = Scalar(0xff, 0x00, 0x94);
		mapColor[player] = Scalar(255, 255, 255);
		InitializeCriticalSection(&cs_visualization);
		return 0;
	}
	//if rect is out of edge,it print beside to the edge
	int printRect(Mat &canvas,const CRectangle &rect,Scalar &color,const CRectangle &edge)
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
		rectangle(canvas,p1,p2,color, CV_FILLED,8);
		return 0;
	}

	Mat DnfRoomState(int narrowRate = 2)
	{
		int dnfWidth = 800;
		int dnfHeight = 600;
		int dnfedgeWidth = 30;

		int dnfScreenWidth = dnfWidth / narrowRate + 2 * dnfedgeWidth;
		int dnfScreenHeight = dnfHeight / narrowRate + 2 * dnfedgeWidth;

		Mat image = Mat::zeros(dnfHeight / narrowRate + 2 * dnfedgeWidth, dnfWidth / narrowRate, CV_8UC3);

		CRectangle screen(dnfedgeWidth, dnfedgeWidth, dnfScreenWidth, dnfScreenHeight);

		if (g_RoomState.m_Player.m_rect.x != 0)
		{

			printRect(image, g_RoomState.m_Player.m_rect, mapColor[player], screen);
		}
		else
		{
			putText(image, "No player!", Point(0, dnfedgeWidth / 2), FONT_HERSHEY_PLAIN, 1,mapColor[player], 1, 8);
		}

		for (auto iter = g_RoomState.m_Monster.m_vecCMon.begin() ; iter != g_RoomState.m_Monster.m_vecCMon.end(); iter++)
		{
			printRect(image, iter->m_rect, mapColor[monster], screen);
		}
		return image;
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
			imshow("dnfScreen", DnfRoomState());
			::LeaveCriticalSection(&cs_visualization);
			int r = waitKey(50);
			if (r != -1)
			{
				controlVisThread = 0;
				destroyWindow("dnfScreen");
			}
		}

		return 0;
	}




}