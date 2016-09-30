#pragma once
#include"stdafx.h"
#include"Room.h"
#include "Neural.h"
#include"Decision.h"
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



	class ActShuangDao :public ActWithArea
	{
	public:
		virtual ActShuangDao* getClassType() { return this; }
		ActShuangDao();

		virtual void run();
		virtual void cal();
		virtual void express();
		CSkShuangDao m_ShuangDao;
		
	};

	class ActZhiChong :public ActWithArea
	{
	public:
		virtual ActZhiChong* getClassType() { return this; }
		ActZhiChong();
		virtual void run();
		virtual void cal();
		virtual void express();
	};

	class ActShiZiZhan :public ActWithArea
	{
	public:
		virtual ActShiZiZhan* getClassType() { return this; }
		virtual void run();
		CSkill *m_skill;
	};

	int loadNeural();

}
