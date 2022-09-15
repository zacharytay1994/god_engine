#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void PhysicsSystem(EnttXSol& entt, std::tuple<Transform&> components)
	{
		// some code here ...
		/*auto& entity_data = std::get<0> ( components );
		Transform* parent_transform = entt.GetEngineComponent<Transform> ( entity_data.m_parent_id );
		if ( parent_transform )
		{
			parent_transform->m_position += 0.002f;
		}*/
	}
}