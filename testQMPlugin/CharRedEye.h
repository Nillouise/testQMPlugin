#pragma once

#include"stdafx.h"
#include"Room.h"
#include "Neural.h"

namespace gandalfr
{
	class CSkShuangDao :public CSkill
	{
	public:
		CSkShuangDao()
		{
			m_cooldown = 350;
			m_NextTime = 0;
			m_Key.push_back(CKeyOp(L"x"));
		}

		std::vector<CAttackArea> getArea(Cdmsoft dm);
	};



	class ActShuangDao :public ActNeural
	{
	public:
		virtual void run();
		virtual void express();

		CSkShuangDao m_ShuangDao;
		std::vector<CAttackArea> m_area;
		std::vector<CTrail> m_vecTrail;


		
	};


}
