#pragma once

#include "../EngineComponents/EC_All.h"
#include "../EnttXSol.h"

namespace god
{
	void GridSystem ( EnttXSol& entt , std::tuple<Transform& , GridCell&> components )
	{
		( entt );
		auto& transform = std::get<0> ( components );
		auto& grid_cell = std::get<1> ( components );

		transform.m_position = {
			( grid_cell.m_cell_x * 2 + 1 ) * grid_cell.m_cell_size,
			( grid_cell.m_cell_y * 2 ) * grid_cell.m_cell_size,
			( grid_cell.m_cell_z * 2 + 1 ) * grid_cell.m_cell_size };
	}
}