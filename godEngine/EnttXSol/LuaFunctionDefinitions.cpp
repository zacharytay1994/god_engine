#include "../pch.h"
#include "LuaFunctionDefinitions.h"

#include "EnttXSol.h"
#include "EngineComponents/EC_All.h"
#include "../Window/DeltaTimer.h"

#include <sol/sol.hpp>
#include <glm/glm/glm.hpp>

namespace god
{
	void RegisterLuaCPP ( EnttXSol& entt , EngineResources& engineResources )
	{
		// glm::vec3
		entt.RegisterLuaType<glm::vec3> ( "vec3" ,
			"x" , &glm::vec3::x ,
			"y" , &glm::vec3::y ,
			"z" , &glm::vec3::z );

		// glm::ivec3
		entt.RegisterLuaType<glm::ivec3> ( "ivec3" ,
			"x" , &glm::ivec3::x ,
			"y" , &glm::ivec3::y ,
			"z" , &glm::ivec3::z );

		// GetComponent(e,componentName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( entt.m_identifier_GetScriptComponent ,
			[&entt]( entt::entity e , std::string const& s )->sol::table&
			{
#if _DEBUG
				if ( !entt.GetStorage<sol::table> ( s ).contains ( e ) )
				{
					std::cout << "LUASCRIPTERROR - Trying to get script component [" << s << "] from entity [" << static_cast< int >( e ) << "] that does not exist." << std::endl;
				}
#endif
				return entt.GetStorage<sol::table> ( s ).get ( e );
			}
		);

		// GetEntity(entityName)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetEntity" ,
			[&entt]( std::string const& entityName )->int
			{
				// potential area for optimization looking for entity of name
				for ( uint32_t i = 0; i < entt.m_entities.Size (); ++i )
				{
					if ( entt.m_entities[ i ].m_name == entityName )
					{
						return static_cast< int >( entt.m_entities[ i ].m_id );
					}
				}
				return -1;
			}
		);

		// GetDeltaTime()
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetDeltaTime" ,
			[]()->float
			{
				return DeltaTimer::m_dt;
			}
		);

		// GetPathToCell(e,x,y,z)
		// ==============================================================================================
		entt.RegisterLuaFunction ( "GetPath" ,
			[&entt , &engineResources]( entt::entity e , int x , int y , int z )-> std::vector<glm::ivec3>
			{
				GridCell* grid_cell = entt.GetEngineComponent<GridCell> ( e );
				EntityData* entity_data = entt.GetEngineComponent<EntityData> ( e );
				if ( grid_cell && entity_data )
				{
					auto& grid = engineResources.Get<EntityGrid> ().get ();
					auto path = grid[ entity_data->m_parent_id ].GetPathAStar (
						grid_cell->m_cell_size ,
						{ grid_cell->m_cell_x, grid_cell->m_cell_y, grid_cell->m_cell_z } ,
						{ x, y, z }
					);

					std::vector<glm::ivec3> out;
					for ( auto const& coordinate : path )
					{
						out.push_back ( { std::get<0> ( coordinate ),std::get<1> ( coordinate ) ,std::get<2> ( coordinate ) } );
					}
					return out;
				}
				return std::vector<glm::ivec3> ();
			}
		);
	}
}