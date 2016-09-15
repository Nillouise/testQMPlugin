#pragma once
#include"stdafx.h"
#include"Cdmsoft.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>
#include<map>



namespace gandalfr
{
	class  CRoomState;

	class CKeyOp
	{
	public:

		std::wstring m_Key;
		DWORD m_KeyTime;
		int(*m_KeyCallback)(int x);//when a key press ,it call back 
		int m_KeyType; // 0 to press,10 to down if this key no down ,then up before down;11 to down ,if down,no down again;20 to up
		int m_forWhat;//0 if for run to some area,1 to release skill,you can you other num to approach other effect
		static int KeyDefaultCallback(int x);
		static std::vector<CKeyOp> m_vecCKeyOp; // record the history key
		static std::priority_queue<CKeyOp, std::vector<CKeyOp>, std::greater<CKeyOp> > m_pqKeyOp;  // incoming key
		static CRITICAL_SECTION g_csKeyOp;
		static std::map<std::wstring, DWORD> m_keyDown;
		CKeyOp(std::wstring Key = L"", DWORD KeyTime = 0, int KeyType = 0, int(*KeyCallback)(int) = KeyDefaultCallback) :m_Key(Key), m_KeyTime(KeyTime), m_KeyCallback(KeyCallback), m_KeyType(KeyType) {}
		static UINT __stdcall KeyboardInput(LPVOID);//use to begin a new thread
	};
	bool operator < (const CKeyOp &t1, const CKeyOp &t2);
	bool operator > (const CKeyOp &t1, const CKeyOp &t2);





	class CRectangle
	{
	public:
		int x;
		int y;
		int width;
		int height;
		CRectangle(int x = -1, int y = -1, int width = 0, int height = 0) :x(x), y(y), width(width), height(height) {};
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
		static CMonsterSet findMonster(Cdmsoft dm, int rangeX = 0, int rangeY = 0, int rangeWidth = 800, int rangeHeight = 600, WCHAR *MonColor = L"ff0094-010101", double similar = 1.0, int PointCount = 250, int monWidth = 30, int monHeight = 30);
	
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

		CPlayer();
		 
		static int findPlayer(Cdmsoft dm);
		static CPlayer getPlayer(Cdmsoft dm);
		int move(CRectangle target);

	};


	class CObstacle
	{
	public:
		std::vector<CRectangle> m_vecObstacle;
		
		static CObstacle getObstacle(Cdmsoft dm);
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
		CRoomState();
		~CRoomState();

		std::vector<CMonsterSet> m_vecMonTrail;
		std::vector<CGoldSet> m_vecGoldTrail;
		std::vector<CPlayer> m_vecPlayerTrail;
		std::vector<CObstacle> m_vecObstacleTrail;
		std::vector<CSceneBox> m_vecSceneBoxTrail;

		CMonsterSet m_monster;
		CGoldSet m_gold;
		CPlayer m_player;
		CObstacle m_Obstacle;
		CSceneBox m_CSceneBox;

		std::map<std::wstring, int> m_runState;//0 to up a key,1 to walk key ,2 to run key;


	private:

	};
	


}

extern gandalfr::CRoomState g_RoomState;