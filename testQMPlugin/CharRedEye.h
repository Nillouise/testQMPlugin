#pragma once

#include"stdafx.h"
#include"Room.h"
#include "Neural.h"

namespace RedEye
{
	class CSkShuangDao :public CSkill
	{
	public:
		
		CSkShuangDao()
		{
			m_cooldown = 0;
			m_NextTime = 0;
			m_Key.push_back(CKeyOp(L"x",0,10)); //x may be change,it should use a abstract layer
		}

		std::vector<CAttackArea> getArea(Cdmsoft dm);
	};



	class ActShuangDao :public ActNeural
	{
	public:
		virtual void run();
		virtual void express();
		virtual ActShuangDao* getClassType() { return this; }
		CSkShuangDao m_ShuangDao;
		std::vector<CAttackArea> m_area;
		CAttackArea m_bestArea;
		std::vector<CTrail> m_vecTrail;
		
	};


}
