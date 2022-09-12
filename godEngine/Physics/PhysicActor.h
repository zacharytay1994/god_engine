#pragma once

#include "../EnttXSol/Internal/godEntity.h"

namespace god
{
	class PhysicActor
	{
	public:
		PhysicActor(Entity_ entity);
		~PhysicActor();

	private:
		Entity_ m_entity;
		
	};
}

