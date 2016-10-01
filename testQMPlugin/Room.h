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

	typedef  std::function<double(DWORD pressTime)> fnKeyPressCallBack;
	extern fnKeyPressCallBack KeyDefaultCallback;
	class CKeyOp
	{
	public:
		enum keyMode { PRESS, DOWMAGAIN, DOWMNOAGAIN, UP };//DOWMAGAIN is if this key down,then up and down again,DOWNNOAGAIN,it only responce to down state,no again

		std::wstring m_Key;
		DWORD m_KeyTime;
//		int(*m_KeyCallback)(DWORD x);//when a key press ,it call back£¬the process while pass the process time
		fnKeyPressCallBack m_KeyCallback;
		keyMode m_KeyType;// this key is press or down or up
		int m_signal;//1 if for run to some area,2 to release skill,you can you other num to approach other effect
	//	DWORD testss;

		static DWORD m_nowTime;
		static bool m_RunTheKeyBoard;//if it is false,the keyboard thread will exit



		static std::vector<CKeyOp> m_hisCKeyOp; // record the history key
		static std::set<CKeyOp > m_setKeyOp;  // incoming key
		static CRITICAL_SECTION g_csKeyOp;
		static std::map<std::wstring, DWORD> m_keyRecentProcess;//the key press in the recent time.if is downing,it also is the dowing time
		static std::map<std::wstring, int> m_keyStateSignal;//the key is 0 if not down,else int represent the ActTemp's keySignal;

		static int fillVecUpRunKeyCurrentTime(std::vector<CKeyOp> &vec, DWORD timeToUpKey);//use in actTemp to up the run key.
																						   //		static int upRunKey(DWORD upTime);// up the left,right,up,down key,if they are downing
		static int UpSlefKeyAndDelKeyNoExe(int signalId);//it no use yet
		static int upKeyNoUpThenClearKeySet(int signalId);// it no clear the m_setKeyOp first
		static int eraseRunKey();//erase all incoming run key

		static void processKey(Cdmsoft dm, const std::wstring &key, const keyMode &mode, const int &signal);//process this key op include set keyStateDown and m_keyStateSignal;

		CKeyOp(std::wstring Key = L"", DWORD KeyTime = 0, keyMode KeyType = CKeyOp::PRESS, fnKeyPressCallBack keyCallBack = KeyDefaultCallback, int signal = 1) :m_Key(Key), m_KeyTime(KeyTime), m_KeyCallback(keyCallBack), m_KeyType(KeyType), m_signal(signal) {}
		static UINT __stdcall KeyboardInput(PVOID);//use to begin a new thread
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
		static int RectCollide(const CRectangle &A, const CRectangle &B, CRectangle* R = NULL);//1 to collide,R is the collided Rectangle
		static int getRectTrail(const CRectangle &player, const CRectangle &rect, CTrail &receive);//player go to the center of rect 's trail

	};
	bool operator < (const CRectangle &t1, const CRectangle &t2);


	class CSpeed
	{
	public:
		int m_spRunX;
		int m_spRunY;
		int m_spWalkX;
		int m_spWalkY;
		CSpeed(int RunX = 275, int RunY = 110, int WalkX = 137, int WalkY = 110) :m_spRunX(RunX), m_spRunY(RunY), m_spWalkX(WalkX), m_spWalkY(WalkY) {};
	};


	class CTrail
	{
	public:
		int x;  // when x bigger than a edge,i.e. runForeverX = 800,it represent the infinity;
		int y; // when y  bigger than a edge,i.e. runForeverY = 600 it represent the infinity;
		DWORD beginTime;//no use
		CRectangle beginArea;//no use
		DWORD endTime;//no use
		CSpeed speed;//no use
		std::vector<CKeyOp> via; //no use, maybe you can use skill to move yourself.
		CTrail(int x = 0, int y = 0, DWORD beginTime = 0, DWORD endTime = 0, CRectangle beginArea = CRectangle()) :x(x), y(y), beginTime(beginTime), endTime(endTime), beginArea(beginArea) {};

	};


	class CAttackArea
	{
	public:
		CRectangle m_rect;
		double direction; //-1.0 represent to left,1.0 represent to right, 0 to both
		std::vector<CMonsterOne> vec_Mons; //no use, how number could attack
		int num;
		double score;
		CAttackArea(CRectangle rect, double direction, int num) :m_rect(rect), direction(direction), num(num) {}
		CAttackArea() { num = 0; score = 0; }
	};


	class CMonsterOne
	{
	public:
		CRectangle m_rect;
		double m_hitTime;   // represent the hit time;
		DWORD m_lastHitedTime;// last hit time
		CTrail m_trail;//this monster's move trail;
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

	};


	class CGoldOne
	{
	public:
		CRectangle m_rect;
		std::wstring m_name;
		int m_type;// it is white or purple or pink?
		CGoldOne(CRectangle m_rect, std::wstring m_name = L"", int m_type = 0) :m_rect(m_rect), m_name(m_name), m_type(m_type) {};
	};


	class CGoldSet
	{
	public:
		std::vector<CGoldOne> m_vecGold;
		DWORD time;

	};


	class CPlayer
	{
	public:
		CRectangle m_rect;
		double m_direction; //-1.0 to +1.0 to point the possible
		DWORD m_time;
		int m_state;//in fuhuo or bati
		int m_HP;
		int m_MP;
		CSpeed m_speed;//default speed:int RunX = 270, int RunY = 110, int WalkX = 110, int WalkY = 110

		CPlayer() { m_rect = CRectangle(0, 0, 30, 20); }
		static CPlayer getPlayer();
	};

	class CObstacleOne
	{
	public:
		CRectangle m_rect;
		CObstacleOne(CRectangle rect) :m_rect(rect) {}
	};

	class CObstacleSet
	{
	public:
		std::vector<CObstacleOne> m_vecObstacle;
		DWORD m_time;


	};

	class CSceneBoxOne
	{
	public:
		CRectangle m_rect;
		CSceneBoxOne(CRectangle rect) :m_rect(rect) {}
	};


	class CSceneBoxSet
	{
	public:
		std::vector<CSceneBoxOne> m_vecCSceneBox;
		DWORD m_time;
	};





	class CDecision
	{
	public:

		static int getMonsterOverlay(const CRectangle &rectSkill, std::vector<std::vector<CRectangle>> &receive, const CMonsterSet &monset);
	};




	class CSkill
	{
	public:
		DWORD m_NextTime;
		DWORD m_lastTime;
		DWORD m_cooldown;
		CRectangle m_area;
		bool canUse();
		DWORD m_HitrecoverTime;
		std::vector<CKeyOp> m_Key;
		int release(DWORD curTime);

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
		std::vector<CObstacleSet> m_vecObstacleTrail;
		std::vector<CSceneBoxSet> m_vecSceneBoxTrail;

		CMonsterSet m_Monster;
		CGoldSet m_Gold;
		CPlayer m_Player;
		CObstacleSet m_Obstacle;
		CSceneBoxSet m_SceneBox;//SceneBox can be attacked

		std::map<std::wstring, int> m_runState;//0 to up a key,1 to walk key ,2 to run key;if > 1,it relative key must down

		void run(Cdmsoft dm);
		int getAllRectStateInRoom(Cdmsoft dm);

		int setRunStateCorrectly();
		double getPlayerDirectionUseKeyOp();

		void clearOldState();
	private:

	};

	//return 1 if coDirection,return 0 if not;
	double isCoDirection(double player, double area);


}

extern gandalfr::CRoomState g_RoomState;