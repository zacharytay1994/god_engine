#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

#include <tuple>

namespace god
{
	void GridSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , Transform& , GridCell&> components )
	{
		( entt );
		auto& entity_data = std::get<0> ( components );
		auto& transform = std::get<1> ( components );
		auto& grid_cell = std::get<2> ( components );

		if ( !grid_cell.m_initialized )
		{
			auto& entity_grid = engineResources.Get<EntityGrid> ().get ();
			entity_grid[ entity_data.m_parent_id ].Insert ( grid_cell.m_cell_size , { grid_cell.m_cell_x,grid_cell.m_cell_y ,grid_cell.m_cell_z } , { entity_data.m_id, grid_cell.m_passable, grid_cell.m_steppable } );
			grid_cell.m_initialized = true;
		}

		transform.m_position = {
			( grid_cell.m_cell_x * 2 + 1 ) * grid_cell.m_cell_size,
			( grid_cell.m_cell_y * 2 ) * grid_cell.m_cell_size,
			( grid_cell.m_cell_z * 2 + 1 ) * grid_cell.m_cell_size };

		// update cell in grid, if cell change detected
		EntityGrid& grid = engineResources.Get<EntityGrid> ().get ();
		if ( grid_cell.m_cell_ox != grid_cell.m_cell_x || grid_cell.m_cell_oy != grid_cell.m_cell_y || grid_cell.m_cell_oz != grid_cell.m_cell_z || grid_cell.m_changed )
		{
			grid[ entity_data.m_parent_id ].ChangeCell ( { entity_data.m_id, grid_cell.m_passable, grid_cell.m_steppable } , grid_cell.m_cell_size , { grid_cell.m_cell_ox, grid_cell.m_cell_oy, grid_cell.m_cell_oz } , { grid_cell.m_cell_x, grid_cell.m_cell_y, grid_cell.m_cell_z } );
			grid_cell.m_cell_ox = grid_cell.m_cell_x;
			grid_cell.m_cell_oy = grid_cell.m_cell_y;
			grid_cell.m_cell_oz = grid_cell.m_cell_z;

			grid_cell.m_changed = false;
		}
	}
}