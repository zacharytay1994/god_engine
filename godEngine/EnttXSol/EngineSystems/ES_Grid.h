#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void GridSystem ( EnttXSol& entt , std::tuple<EntityData& , Transform& , GridCell&> components )
	{
		auto& entity_data = std::get<0> ( components );
		auto& transform = std::get<1> ( components );
		auto& grid_cell = std::get<2> ( components );

		// calculate parent offset
		/*glm::vec3 offset { 0.0f };
		Transform* parent_transform = entt.GetEngineComponent<Transform> ( entity_data.m_parent_id );
		if ( parent_transform )
		{
			parent_transform.
		}*/

		transform.m_position = { 
			( grid_cell.m_cell_x * 2 + 1 ) * grid_cell.m_cell_size, 
			transform.m_position.y,
			( grid_cell.m_cell_z * 2 + 1 ) * grid_cell.m_cell_size };
	}
}