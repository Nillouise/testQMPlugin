#pragma once
#include"stdafx.h"
#include"Room.h"
#include "Neural.h"
#include"Decision.h"

namespace BaoPo
{

	class ActPingX :public ActWithArea
	{
	public:
		virtual ActPingX* getClassType() { return this; }
		ActPingX();
		CSkill *m_skill;
		ActPingX(CSkill * skill) :m_skill(skill) { m_base = 2; }
		virtual void run();
		virtual void cal();
		virtual void express();

	};

	class AttackSkill :public CSkill
	{
	public:
	};



	class ActHalfSkill :public ActWithArea
	{
	public:
		virtual ActHalfSkill* getClassType() { return this; }
		virtual void run();
		virtual void cal();
		virtual void express();


		ActHalfSkill(CSkill * skill)  { m_skill = skill;  m_base = 2; }
	};






	int loadNeural();

}
