#pragma once
#include"stdafx.h"
#include"Cdmsoft.h"
#include<sstream>
#include<fstream>
#include<queue>
#include<functional>
#include<cmath>


namespace gandalfr
{
	class CRoomState
	{
	public:
		CRoomState();
		~CRoomState();

	private:

	};


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

	class CMonster
	{
	public:
		CRectangle m_rect;
		int m_hitTime;
		DWORD m_time;
		CMonster *m_nextTrail;
		CMonster *m_preTrail;
		static std::vector<CMonster> m_vecCMon;
		static std::vector<std::vector<CMonster> > m_vecCMonTrail;
		CMonster(CRectangle rect, int hitTime = 0) :m_rect(rect), m_hitTime(hitTime) {};
		static long findMonster(Cdmsoft dm, int rangeX = 0, int rangeY = 0, int rangeWidth = 800, int rangeHeight = 600, WCHAR *MonColor = L"ff0094-010101", double similar = 1.0, int PointCount = 250, int monWidth = 30, int monHeight = 30);
	};
	

	class CKeyOp
	{
	public:

		std::wstring m_Key;
		DWORD m_KeyTime;
		int(*m_KeyCallback)(int x);
		int m_KeyType;
		static int KeyDefaultCallback(int x);
		static std::vector<CKeyOp> m_vecCKeyOp;
		static std::priority_queue<CKeyOp, std::vector<CKeyOp>, std::greater<CKeyOp> > m_pqKeyOp;
		static CRITICAL_SECTION g_csKeyOp;
		CKeyOp(std::wstring Key = L"", DWORD KeyTime = 0, int KeyType = 0, int(*KeyCallback)(int) = KeyDefaultCallback) :m_Key(Key), m_KeyTime(KeyTime), m_KeyCallback(KeyCallback), m_KeyType(KeyType) {}
		static UINT __stdcall KeyboardInput(LPVOID);
	};
	bool operator < (const CKeyOp &t1, const CKeyOp &t2);
	bool operator > (const CKeyOp &t1, const CKeyOp &t2);


	class CSpeed
	{
		int m_spRunX;
		int m_spRunY;
		int m_spWalkX;
		int m_spWalkY;
		CSpeed(int RunX = 953, int RunY = 110, int WalkX = 300, int WalkY = 110) :m_spRunX(RunX), m_spRunY(RunY), m_spWalkX(WalkX), m_spWalkY(WalkY) {};
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
		CPlayer();

		static std::vector<CPlayer> m_vecPlayer;
		static int findPlayer(Cdmsoft dm);
		static CPlayer getPlayer(Cdmsoft dm);
		int move(CRectangle target);

	};

	class CSkillValue
	{
		double value;
		std::vector<CRectangle> rects;



	};

	class CSkill
	{
		DWORD m_NextTime;
		std::vector<CKeyOp> m_Key;


	};

	class CCharacter
	{
		CSpeed m_speed;
	};

	class CRedEye :public CCharacter
	{


	};

	class CDecision
	{
	public:
		CRectangle MostMonsterPoint();
		CRectangle MostMonsterInRect(std::vector<CRectangle> rect);
		CRectangle MostMonsterNearlyPlayer(CPlayer player, CRectangle range);

		static int getMonsterOverlay(CRectangle rectSkill, std::vector<std::vector<CRectangle>> &receive);


	};


}