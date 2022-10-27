#pragma once
#include "../Editor.h"
#include "../../OpenGL/OpenGL.h"
#include "../../imgui/imgui_stdlib.h" 
#include "../../Window/GLFWWindow.h"
#include <godUtility/Grid3D.h>
#include <godCamera/Camera.h>
#include <glm/glm/glm.hpp>
#include <cmath>
#include <numeric>
#include <limits>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_TilemapEditor : EditorWindow<EDITOR_RESOURCES>
	{
		EW_TilemapEditor ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& engineResources ) override;

		int m_y_level { 0 };
		int m_tilemap_dimensions { 50 };
		float m_cell_size { 1 };
		int m_cell_x { 0 } , m_cell_y { 0 } , m_cell_z { 0 };
		int m_placement_mode { 1 };
		bool m_show_selected_grid { false } , m_show_parent_grid { true };
		bool m_free_placement { false } , m_replace_cell { true };
		EnttXSol::Entities::ID m_selected { EnttXSol::Entities::Null };

		const float INF { 1'000'000.0f };

		glm::vec4 m_line_colour { 0.3f, 0.3f, 0.3f, 0.7f };

		std::vector<std::string> m_prefab_list;
		std::string m_selected_prefab { "-None-" };

		EnttXSol& m_enttxsol;

		EnttXSol::Entities::ID m_preview_id { EnttXSol::Entities::Null };

		void ClearPreview ( EDITOR_RESOURCES& engineResources );
	};
}

#include "EW_SceneTree.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_TilemapEditor<EDITOR_RESOURCES>::EW_TilemapEditor ( EnttXSol& enttxsol )
		:
		m_enttxsol { enttxsol }
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_TilemapEditor<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& engineResources )
	{
		( dt );
		EntityGrid& grid = engineResources.Get<EntityGrid> ().get ();

		float m_true_cell_size { m_cell_size * 2 };
		float tilemap_dimension { m_tilemap_dimensions * m_true_cell_size };

		// origin
		float ox { 0.0f } , oy { 0.0f } , oz { 0.0f } , osx { 1.0f } , osy { 1.0f } , osz { 1.0f };
		if ( m_selected != EnttXSol::Entities::Null )
		{
			Transform* transform = m_enttxsol.GetEngineComponent<Transform> ( m_selected );
			if ( transform )
			{
				auto position = transform->m_parent_transform * glm::vec4 ( transform->m_position , 1.0f );
				auto scale = transform->m_parent_transform * glm::vec4 ( transform->m_scale , 0.0f );
				ox = position.x;
				oy = position.y;
				oz = position.z;
				osx = scale.x;
				osy = scale.y;
				osz = scale.z;
			}

			// draw axes at parent object
			EntityData* selected_entity_data = m_enttxsol.GetEngineComponent<EntityData> ( m_selected );
			if ( selected_entity_data )
			{
				Transform* parent_transform = m_enttxsol.GetEngineComponent<Transform> ( selected_entity_data->m_parent_id );
				glm::vec3 grid_position { 0.0f };
				glm::vec3 grid_scale { 1.0f };
				if ( parent_transform )
				{
					grid_position = parent_transform->m_parent_transform * glm::vec4 ( parent_transform->m_position , 1.0f );
					grid_scale = parent_transform->m_parent_transform * glm::vec4 ( parent_transform->m_scale , 0.0f );
				}

				// draw parent grid
				if ( m_show_parent_grid )
				{
					float true_parent_cell_size { 1.0f * 2 };
					float parent_tilemap_dimension { m_tilemap_dimensions * true_parent_cell_size };

					GridCell* grid_cell = m_enttxsol.GetEngineComponent<GridCell> ( m_selected );
					if ( grid_cell )
					{
						true_parent_cell_size = grid_cell->m_cell_size * 2.0f;
						parent_tilemap_dimension = m_tilemap_dimensions * true_parent_cell_size;
					}

					float parent_grid_y { grid_position.y };
					if ( transform )
					{
						auto position = transform->m_parent_transform * glm::vec4 ( transform->m_position , 1.0f );
						parent_grid_y = position.y;
					}

					// parent axes
					// x-axis
					OpenGL::DrawLine ( { grid_position.x, parent_grid_y, INF } , { grid_position.x, parent_grid_y, -INF } , { 0.0f, 0.0f, 0.6f, 0.8f } , 3.0f );
					// y-axis
					OpenGL::DrawLine ( { grid_position.x, INF, grid_position.z } , { grid_position.x, -INF, grid_position.z } , { 0.0f, 0.6f, 0.0f, 0.8f } , 3.0f );
					// z-axis
					OpenGL::DrawLine ( { INF, parent_grid_y, grid_position.z } , { -INF, parent_grid_y, grid_position.z } , { 0.6f, 0.0f, 0.0f, 0.6f } , 3.0f );

					// child axes
					// x-axis
					OpenGL::DrawLine ( { grid_position.x * true_parent_cell_size * grid_scale.x, parent_grid_y, INF } , { grid_position.x * true_parent_cell_size * grid_scale.x, parent_grid_y, -INF } , { 0.0f, 0.0f, 0.6f, 0.8f } , 3.0f );
					// z-axis
					OpenGL::DrawLine ( { INF, parent_grid_y, grid_position.z * true_parent_cell_size * grid_scale.z } , { -INF, parent_grid_y, grid_position.z * true_parent_cell_size * grid_scale.z } , { 0.6f, 0.0f, 0.0f, 0.8f } , 3.0f );

					for ( int i = 1; i < m_tilemap_dimensions; ++i )
					{
						// vertical
						OpenGL::DrawLine ( { grid_position.x + i * true_parent_cell_size * grid_scale.x, parent_grid_y , grid_position.z + parent_tilemap_dimension * true_parent_cell_size * grid_scale.z } , { grid_position.x + i * true_parent_cell_size * grid_scale.x , parent_grid_y, grid_position.z - parent_tilemap_dimension * true_parent_cell_size * grid_scale.z } , m_line_colour );

						OpenGL::DrawLine ( { grid_position.x - i * true_parent_cell_size * grid_scale.x, parent_grid_y , grid_position.z + parent_tilemap_dimension * true_parent_cell_size * grid_scale.z } , { grid_position.x - i * true_parent_cell_size * grid_scale.x , parent_grid_y, grid_position.z - parent_tilemap_dimension * true_parent_cell_size * grid_scale.z } , m_line_colour );

						// horizontal
						OpenGL::DrawLine ( { grid_position.x + parent_tilemap_dimension * true_parent_cell_size * grid_scale.x , parent_grid_y , grid_position.z + i * true_parent_cell_size * grid_scale.z } , { grid_position.x - parent_tilemap_dimension * true_parent_cell_size * grid_scale.x , parent_grid_y, grid_position.z + i * true_parent_cell_size * grid_scale.z } , m_line_colour );

						OpenGL::DrawLine ( { grid_position.x + parent_tilemap_dimension * true_parent_cell_size * grid_scale.x , parent_grid_y , grid_position.z - i * true_parent_cell_size * grid_scale.z } , { grid_position.x - parent_tilemap_dimension * true_parent_cell_size * grid_scale.x , parent_grid_y, grid_position.z - i * true_parent_cell_size * grid_scale.z } , m_line_colour );

					}
				}
			}
		}

		float child_y = oy + ( static_cast< float >( m_cell_y ) * m_true_cell_size ) * osy;

		// get tile mouse is on
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
		if ( IntersectLineSegmentPlane ( a , b , { 0,1,0 } , child_y , intersect ) )
		{
			m_cell_x = static_cast< int >( std::floor ( ( intersect.x - ox ) / ( m_true_cell_size * osx ) ) );
			m_cell_z = static_cast< int >( std::floor ( ( intersect.z - oz ) / ( m_true_cell_size * osz ) ) );
		}

		glm::vec3 intersect_local { ( intersect.x - ox ) / ( osx ), ( intersect.y - oy ) / ( osy ), ( intersect.z - oz ) / ( osz ) };

		// draw grid at mouse position
		if ( m_show_selected_grid )
		{
			// draw axes at selected object
			// x-axis
			OpenGL::DrawLine ( { ox, child_y, INF } , { ox, child_y, -INF } , { 0.0f, 0.0f, 0.6f, 0.8f } , 3.0f );
			// y-axis
			OpenGL::DrawLine ( { ox, INF, oz } , { ox, -INF, oz } , { 0.0f, 0.6f, 0.0f, 0.8f } , 3.0f );
			// z-axis
			OpenGL::DrawLine ( { INF, child_y, oz } , { -INF, child_y, oz } , { 0.6f, 0.0f, 0.0f, 0.6f } , 3.0f );

			// x-axis
			OpenGL::DrawLine ( { ox + m_cell_x * m_true_cell_size * osx, child_y, INF } , { ox + m_cell_x * m_true_cell_size * osx, child_y, -INF } , { 0.0f, 0.0f, 0.6f, 0.8f } , 3.0f );
			// z-axis
			OpenGL::DrawLine ( { INF, child_y, oz + m_cell_z * m_true_cell_size * osz } , { -INF, child_y, oz + m_cell_z * m_true_cell_size * osz } , { 0.6f, 0.0f, 0.0f, 0.8f } , 3.0f );

			for ( int i = 1; i < m_tilemap_dimensions; ++i )
			{
				// vertical
				if ( i + m_cell_x != 0 )
				{
					OpenGL::DrawLine ( { ox + ( i + m_cell_x ) * m_true_cell_size * osx, child_y , oz + tilemap_dimension + m_cell_z * m_true_cell_size * osz } , { ox + ( i + m_cell_x ) * m_true_cell_size * osx , child_y, oz - tilemap_dimension + m_cell_z * m_true_cell_size * osz } , m_line_colour );
				}
				if ( -i + m_cell_x != 0 )
				{
					OpenGL::DrawLine ( { ox + ( -i + m_cell_x ) * m_true_cell_size * osx, child_y , oz + tilemap_dimension + m_cell_z * m_true_cell_size * osz } , { ox + ( -i + m_cell_x ) * m_true_cell_size * osx , child_y, oz - tilemap_dimension + m_cell_z * m_true_cell_size * osz } , m_line_colour );
				}
				// horizontal
				if ( i + m_cell_z != 0 )
				{
					OpenGL::DrawLine ( { ox + tilemap_dimension + m_cell_x * m_true_cell_size * osx , child_y , oz + ( i + m_cell_z ) * m_true_cell_size * osz } , { ox - tilemap_dimension + m_cell_x * m_true_cell_size * osx , child_y, oz + ( i + m_cell_z ) * m_true_cell_size * osz } , m_line_colour );
				}
				if ( -i + m_cell_z != 0 )
				{
					OpenGL::DrawLine ( { ox + tilemap_dimension + m_cell_x * m_true_cell_size * osx , child_y , oz + ( -i + m_cell_z ) * m_true_cell_size * osz } , { ox - tilemap_dimension + m_cell_x * m_true_cell_size * osx , child_y, oz + ( -i + m_cell_z ) * m_true_cell_size * osz } , m_line_colour );
				}
			}

			OpenGL::DrawLine ( { ox + m_cell_x * m_true_cell_size * osx, child_y, oz + m_cell_z * m_true_cell_size * osz } , { ox + ( m_cell_x + 1 ) * m_true_cell_size * osx, child_y, oz + ( m_cell_z + 1 ) * m_true_cell_size * osz } , { 1,0,0,1 } , 3.0f );
			OpenGL::DrawLine ( { ox + ( m_cell_x + 1 ) * m_true_cell_size * osx, child_y, oz + m_cell_z * m_true_cell_size * osz } , { ox + m_cell_x * m_true_cell_size * osx, child_y, oz + ( m_cell_z + 1 ) * m_true_cell_size * osz } , { 1,0,0,1 } , 3.0f );
		}

		if ( window.KeyDown ( GLFW_KEY_LEFT_CONTROL ) && window.MouseLPressed () && window.WithinWindow () && m_selected_prefab != "-None-" )
		{
			glm::vec3 placement_position { intersect_local };
			// free placement of objects
			if ( !m_free_placement )
			{
				placement_position = { ( static_cast< float >( m_cell_x ) + 0.5f ) * m_true_cell_size,
				static_cast< float >( m_cell_y ) * m_true_cell_size,
				( static_cast< float >( m_cell_z ) + 0.5f ) * m_true_cell_size };
			}
			auto entity = m_enttxsol.AddPrefabToScene ( engineResources , m_selected_prefab , m_selected , placement_position );

			// if object being placed belongs to the grid
			if ( !m_free_placement )
			{
				// erase entities in cell
				if ( m_replace_cell )
				{
					grid[ m_selected ].RunOver ( m_cell_size , { m_cell_x, m_cell_y, m_cell_z } , [&grid]( uint32_t e , EnttXSol& entt ) { entt.RemoveEntity ( grid , e ); } , std::ref ( m_enttxsol ) );
				}

				m_enttxsol.AttachComponent<GridCell> ( entity );
				GridCell* grid_cell = m_enttxsol.GetEngineComponent<GridCell> ( entity );
				grid_cell->m_cell_x = m_cell_x;
				grid_cell->m_cell_y = m_cell_y;
				grid_cell->m_cell_z = m_cell_z;
				grid_cell->m_cell_size = m_cell_size;

				//grid[ m_selected ].Insert ( m_cell_size , { m_cell_x, m_cell_y, m_cell_z } , entity );
			}
		}

		// move the preview to the right cell
		if ( m_preview_id != EnttXSol::Entities::Null )
		{
			Transform* transform = m_enttxsol.GetEngineComponent<Transform> ( m_preview_id );
			if ( transform )
			{
				if ( m_free_placement )
				{
					transform->m_position = intersect_local;
				}
				else
				{
					transform->m_position = {
						( static_cast< float >( m_cell_x ) + 0.5f ) * m_true_cell_size,
						static_cast< float >( m_cell_y ) * m_true_cell_size,
						( static_cast< float >( m_cell_z ) + 0.5f ) * m_true_cell_size };

					GridCell* grid_cell = m_enttxsol.GetEngineComponent<GridCell> ( m_preview_id );
					if ( grid_cell )
					{
						/*grid_cell->m_cell_x = m_cell_x;
						grid_cell->m_cell_y = m_cell_y;
						grid_cell->m_cell_z = m_cell_z;
						grid_cell->m_cell_size = m_cell_size;*/
					}
				}
			}
		}

		ImGui::Begin ( "Tilemap Editor" );

		ImGui::RadioButton ( "Move Mode" , &m_placement_mode , 0 );
		this->ToolTipOnHover ( "Shows the grid the selected object is on.\nTip: Useful for moving selected Objects." );
		ImGui::SameLine ();
		ImGui::RadioButton ( "Place Mode" , &m_placement_mode , 1 );
		this->ToolTipOnHover ( "Preview on grid relative to the selected object.\nTip: Useful for placing new Objects" );
		if ( m_placement_mode == 0 )
		{
			m_show_parent_grid = true;
			m_show_selected_grid = false;
		}
		else if ( m_placement_mode == 1 )
		{
			m_show_parent_grid = false;
			m_show_selected_grid = true;
		}

		ImGui::Checkbox ( "Free Placement" , &m_free_placement );
		ImGui::SameLine ();
		ImGui::Checkbox ( "Replace" , &m_replace_cell );

		ImGui::DragInt ( "Y" , &m_cell_y , 1 );
		ImGui::DragInt ( "Dimension" , &m_tilemap_dimensions , 1 , 1 , 100 , "%.3f" , 0 );
		ImGui::DragFloat ( "Cell Size" , &m_cell_size , 0.1f , 0.1f , 100 , "%.3f" , 0 );

		auto old_selected { m_selected };
		m_selected = this->Get<EW_SceneTree> ()->GetSelectedEntity ();

		// if parent changed and there is a preview update preview
		if ( old_selected != m_selected )
		{
			m_y_level = 0;
			if ( m_preview_id != EnttXSol::Entities::Null && m_enttxsol.m_entities.Valid ( m_preview_id ) )
			{
				m_enttxsol.RemoveEntity ( grid , m_preview_id );
			}
			// if there is a selected prefab brush load the selected prefab as a preview
			if ( m_selected_prefab != "-None-" )
			{
				m_preview_id = m_enttxsol.AddPrefabToScene ( engineResources , m_selected_prefab , m_selected ,
					{ ( static_cast< float >( m_cell_x ) + 0.5f ) * m_true_cell_size,
					static_cast< float >( m_cell_y ) * m_true_cell_size,
					( static_cast< float >( m_cell_z ) + 0.5f ) * m_true_cell_size } ,
					false );
				//m_enttxsol.m_entities[ m_preview_id ].m_persist_in_scene = false;
			}
		}

		// prefab select
		if ( ImGui::BeginPopup ( "PrefabBrushPopup" ) )
		{

			if ( ImGui::Selectable ( "-None-" ) )
			{
				if ( m_preview_id != EnttXSol::Entities::Null && m_enttxsol.m_entities.Valid ( m_preview_id ) )
				{
					m_enttxsol.RemoveEntity ( grid , m_preview_id );
					m_preview_id = EnttXSol::Entities::Null;
				}
				m_selected_prefab = "-None-";
				ImGui::CloseCurrentPopup ();
			}

			for ( auto const& prefab : m_prefab_list )
			{
				auto last_dash = prefab.find_last_of ( '/' );
				auto last_dot = prefab.find_last_of ( '.' );
				std::string name = prefab.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
				if ( ImGui::Selectable ( name.c_str () ) )
				{
					// remove old preview if any
					if ( m_preview_id != EnttXSol::Entities::Null && m_enttxsol.m_entities.Valid ( m_preview_id ) )
					{
						m_enttxsol.RemoveEntity ( grid , m_preview_id );
						m_preview_id = EnttXSol::Entities::Null;
					}

					m_selected_prefab = name;

					// if there is a selected prefab brush load the selected prefab as a preview
					if ( m_selected_prefab != "-None-" )
					{
						m_preview_id = m_enttxsol.AddPrefabToScene ( engineResources , m_selected_prefab , m_selected ,
							{ ( static_cast< float >( m_cell_x ) + 0.5f ) * m_true_cell_size,
							static_cast< float >( m_cell_y ) * m_true_cell_size,
							( static_cast< float >( m_cell_z ) + 0.5f ) * m_true_cell_size } ,
							false );
						//m_enttxsol.m_entities[ m_preview_id ].m_persist_in_scene = false;
					}

					ImGui::CloseCurrentPopup ();
				}
			}

			ImGui::EndPopup ();
		}
		if ( ImGui::Button ( ( std::string ( "Prefab Brush: " ) + m_selected_prefab ).c_str () , { ImGui::GetWindowWidth (), 0 } ) )
		{
			m_prefab_list = FolderHelper::GetFiles ( "Assets/GameAssets/Prefabs/" );
			ImGui::OpenPopup ( "PrefabBrushPopup" );
		}

		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_TilemapEditor<EDITOR_RESOURCES>::ClearPreview ( EDITOR_RESOURCES& engineResources )
	{
		if ( m_preview_id != EnttXSol::Entities::Null && m_enttxsol.m_entities.Valid ( m_preview_id ) )
		{
			m_enttxsol.RemoveEntity ( engineResources.Get<EntityGrid> ().get () , m_preview_id );
			m_preview_id = EnttXSol::Entities::Null;
		}
		m_selected_prefab = "-None-";
	}
}