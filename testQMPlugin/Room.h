#pragma once
#include"stdafx.h"
#include"Cdmsoft.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>
#include<map>
#include<set>



namespace gandalfr
{
	class CRoomState;
	class CMonsterOne;
	class CTrail;

	class CKeyOp
	{
	public:
		enum keyMode { PRESS,DOWMAGAIN,DOWMNOAGAIN,UP};//DOWMAGAIN is if this key down,then up and down again,DOWNNOAGAIN,it only responce to down state,no again
		std::wstring m_Key;
		DWORD m_KeyTime;
		int(*m_KeyCallback)(int x);//when a key press ,it call back 
		keyMode m_KeyType; // 1 to press,10 to down if this key no down ,then up before down;11 to down ,if down,no down again;20 to up
		int m_signal;//0 if for run to some area,1 to release skill,you can you other num to approach other effect
		static int KeyDefaultCallback(int x);
		static std::vector<CKeyOp> m_vecCKeyOp; // record the history key
		static std::set<CKeyOp > m_setKeyOp;  // incoming key
		static CRITICAL_SECTION g_csKeyOp;
		static std::map<std::wstring, DWORD> m_keyStateDown;//the key press in the recent time.if is downing,it should be a max time;
		static std::map<std::wstring, int> m_keyStateSignal;//the key is 0 if not down,else int represent the ActTemp's keySignal;

		static int upRunKey(DWORD upTime);// up the left,right,up,down key,if they are downing
		static int delKeyNoExe(int signalId);
		static int upKeyNoUp(int signalId);

		CKeyOp(std::wstring Key = L"", DWORD KeyTime = 0, keyMode KeyType = CKeyOp::PRESS, int(*KeyCallback)(int) = KeyDefaultCallback) :m_Key(Key), m_KeyTime(KeyTime), m_KeyCallback(KeyCallback), m_KeyType(KeyType),m_signal(0) {}
		static UINT __stdcall KeyboardInput(LPVOID);//use to begin a new thread
	};
	bool operator < (const CKeyOp &t1, const CKeyOp &t2);





	class CRectangle
	{
	public:
		int x;
		int y;
		int width;
		int height;
		CRectangle(int x = -1, int y = -1, int width = 0, int height = 0) :x(x), y(y), width(width), height(height) {};
		static int RectCollide(const CRectangle &A, const CRectangle &B, CRectangle* R = NULL);
		static int getRectTrail(const CRectangle &player, const CRectangle &rect, CTrail &receive);

	};
	bool operator < (const CRectangle &t1, const CRectangle &t2);


	class CSpeed
	{
	public:
		int m_spRunX;
		int m_spRunY;
		int m_spWalkX;
		int m_spWalkY;
		CSpeed(int RunX = 953, int RunY = 110, int WalkX = 300, int WalkY = 110) :m_spRunX(RunX), m_spRunY(RunY), m_spWalkX(WalkX), m_spWalkY(WalkY) {};
	};


	class CTrail
	{
	public:
		int x;  // when x bigger than a edge,it represent the infinity;
		int y; // when y  bigger than a edge,it represent the infinity;
		DWORD beginTime;
		CRectangle beginArea;
		DWORD endTime;
		CSpeed speed;
		std::vector<CKeyOp> via; //maybe you can use skill to move yourself.
		CTrail(int x = 0, int y = 0, DWORD beginTime = 0, DWORD endTime = 0,CRectangle beginArea= CRectangle()) :x(x), y(y), beginTime(beginTime), endTime(endTime),beginArea(beginArea) {};

	};


	class CAttackArea
	{
	public:
		CRectangle m_rect;
		double direction; //-1.0 represent to left,1.0 represent to right, 0 to both
		std::vector<CMonsterOne> vec_Mons; // how number could attack
		int num;
		CAttackArea(CRectangle rect, double direction, int num) :m_rect(rect), direction(direction), num(num) {}
		CAttackArea() {}
	};


	class CMonsterOne
	{
	public:
		CRectangle m_rect;
		double m_hitTime;   // represent the hit time;
		DWORD m_lastHitedTime;// last hit time
		CTrail m_trail;
		double m_bati;  // -1.0 to 1.0 represent the bati probility;
		double m_attackState; // -1.0 to 1.0 represent if it is attacking you;  
		int m_type;  //represent if it is a boss monster or something 
		CMonsterOne(CRectangle rect, int hitTime = 0) :m_rect(rect), m_hitTime(hitTime) {};
	};
	

	class CMonsterSet
	{
	public:
		std::vector<CMonsterOne> m_vecCMon;
		DWORD m_time;
		static CMonsterSet getMonsterSet(Cdmsoft dm, int rangeX = 0, int rangeY = 0, int rangeWidth = 800, int rangeHeight = 600, WCHAR *MonColor = L"ff0094-010101", double similar = 1.0, int PointCount = 250, int monWidth = 30, int monHeight = 30);
	
	};


	class CGoldOne
	{
	public:
		CRectangle m_rect;
		std::wstring m_name;
		int m_type;// it is white or purple or pink?
		CGoldOne(CRectangle m_rect,std::wstring m_name = L"",int m_type = 0) :m_rect(m_rect),m_name(m_name), m_type(m_type) {};
	};


	class CGoldSet
	{
	public:
		std::vector<CGoldOne> m_vecCMon;
		DWORD time;

		static CGoldSet getGoldSet(Cdmsoft dm);
	};


	class CPlayer
	{
	public:
		CRectangle m_rect;
		double m_direction; //-1.0 to +1.0 to point the possible
		DWORD m_time;
		int m_state;
		int m_HP;
		int m_MP;
		CSpeed m_speed;

		 
		static CPlayer getPlayer(Cdmsoft dm);

	};


	class CObstacle
	{
	public:
		std::vector<CRectangle> m_vecObstacle;

		static CObstacle getObstacle(Cdmsoft dm, int rangeX = 0, int rangeY = 0,int rangeWidth = 800,int rangeHeight =600,WCHAR * ObsColor = L"bbccff-010101",double similar =1.0,int PointCount = 2800,int obsWidth =80,int obsHeight = 40);
		DWORD m_time;

	};


	class CSceneBox
	{
	public:
		std::vector<CRectangle> m_vecCSceneBox;
		
		static CSceneBox getSceneBox(Cdmsoft dm);
		DWORD m_time;
	};





	class CDecision
	{
	public:
		CRectangle MostMonsterPoint();
		CRectangle MostMonsterInRect(std::vector<CRectangle> rect);
		CRectangle MostMonsterNearlyPlayer(CPlayer player, CRectangle range);

		static int getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive, const CMonsterSet &monset );
	};


	class CExplore
	{
	public:


	};


	class CSkill
	{
	public:
		DWORD m_NextTime;
		DWORD m_cooldown;
		std::vector<CKeyOp> m_Key;


	};


	class CCharacter
	{
		CSpeed m_speed;
	};


	class CRoomState
	{
	public:

		std::vector<CMonsterSet> m_vecMonTrail;
		std::vector<CGoldSet> m_vecGoldTrail;
		std::vector<CPlayer> m_vecPlayerTrail;
		std::vector<CObstacle> m_vecObstacleTrail;
		std::vector<CSceneBox> m_vecSceneBoxTrail;

		CMonsterSet m_monster;
		CGoldSet m_gold;
		CPlayer m_player;
		CObstacle m_Obstacle;
		CSceneBox m_CSceneBox;//SceneBox can be attacked

		std::map<std::wstring, int> m_runState;//0 to up a key,1 to walk key ,2 to run key;if > 1,it relative key must down

		void run(Cdmsoft dm);

	private:

	};
	
	//return 1 if coDirection,return 0 if not;
	int isCoDirection(double player, double area);


}

extern gandalfr::CRoomState g_RoomState;