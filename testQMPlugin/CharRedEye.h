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
			m_Key.push_back(CKeyOp(L"x",0, CKeyOp::DOWMAGAIN)); //x may be change,it should use a abstract layer
		}
	};



	class ActShuangDao :public ActNeural
	{
	public:
		virtual ActShuangDao* getClassType() { return this; }
		ActShuangDao();

		virtual void run();
		virtual void cal();
		virtual void express();
		CSkShuangDao m_ShuangDao;
		std::vector<CAttackArea> m_area;//possible attack area;
		CAttackArea m_bestArea;
		std::vector<CTrail> m_vecTrail;//go to the m_bestArea's trail
		
	};
	int loadNeural();

}
