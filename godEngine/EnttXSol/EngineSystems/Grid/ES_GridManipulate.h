#pragma once

#include "../../EngineComponents/EC_All.h"
#include "../../EnttXSol.h"

#include <godUtility/FileIO.h>

namespace god
{
	void GridManipulateSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , GridManipulate&> components )
	{
		( entt );
		( components );
		( engineResources );
		// some code here ...

		auto& entity_data = std::get<0> ( components );
		auto& grid_manipulate = std::get<1> ( components );
		auto grid_root_entity = entt.GetEntity ( grid_manipulate.m_grid_root );

		grid_manipulate.m_clicked = false;

		EntityGrid& grid = engineResources.Get<EntityGrid> ().get ();
		GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();
		Camera& camera = engineResources.Get<Camera> ().get ();

		glm::vec3 ray_dir = ViewportToWorldRay (
			{ window.ViewportMouseX (), window.ViewportMouseY () } ,
			window.GetWindowWidth () ,
			window.GetWindowHeight () ,
			camera.GetPerpectiveProjectionMatrix () ,
			camera.GetCameraViewMatrix () );
		glm::vec3 a = camera.m_position , b = camera.m_position + ray_dir * 1000.0f;
		glm::vec3 intersect;

		if ( grid_root_entity != EnttXSol::Entities::Null )
		{
			Transform* grid_transform = entt.GetEngineComponent<Transform> ( grid_root_entity );
			if ( grid_transform )
			{
				Coordinate cell;
				if ( grid[ grid_root_entity ].RayCastToGrid ( cell ,
					grid_transform->m_parent_transform * glm::vec4 ( grid_transform->m_position , 1.0f ) ,
					grid_transform->m_parent_transform * glm::vec4 ( grid_transform->m_scale , 1.0f ) ,
					1.0f , a , b ) )
				{
					if ( grid_manipulate.m_highlight_id != EnttXSol::Entities::Null )
					{
						Transform* transform = entt.GetEngineComponent<Transform> ( grid_manipulate.m_highlight_id );
						if ( transform )
						{
							transform->m_position = { ( std::get<0> ( cell ) + 0.5f ) * 2.0f, ( std::get<1> ( cell ) + 1 ) * 2.0f, ( std::get<2> ( cell ) + 0.5f ) * 2.0f };
						}
					}

					if ( window.MouseLPressed () )
					{
						grid_manipulate.m_clicked = true;
						grid_manipulate.m_last_clicked_cell = { std::get<0> ( cell ), std::get<1> ( cell ) + 1 , std::get<2> ( cell ) };
					}
				}
			}
		}
	}

	void GridManipulateInit ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , GridManipulate&> components )
	{
		( entt );
		( components );
		( engineResources );
		//some code here ...
		//std::cout << "Init - Entities with EntityData & Transform." << std::endl;

		auto& grid_manipulate = std::get<1> ( components );
		auto grid_root_entity = entt.GetEntity ( grid_manipulate.m_grid_root );
		if ( grid_root_entity != EnttXSol::Entities::Null && FileExists ( "Assets/GameAssets/Prefabs/" , grid_manipulate.m_highlight_prefab + ".json" ) )
		{
			grid_manipulate.m_highlight_id = entt.AddPrefabToScene ( engineResources , grid_manipulate.m_highlight_prefab , grid_root_entity );
			entt.m_entities[ grid_manipulate.m_highlight_id ].m_persist_in_scene = false;
		}
	}
}