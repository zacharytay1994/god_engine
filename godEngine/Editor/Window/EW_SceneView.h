#pragma once
#include "../Editor.h"
#include "../../EnttXSol/EnttXSol.h"
#include "../../EnttXSol/EngineComponents/EC_All.h"
#include "../../Window/GLFWWindow.h"
#include "../../Window/DeltaTimer.h"
#include "../../Audio/AudioAPI.h"
#include <godCamera/Camera.h>
#include <godUtility/Math.h>
#include <godUtility/Grid3D.h>

#include "../../imgui/imgui_stdlib.h" 
#include "../../imgui/ImGuizmo.h"

#include <glm/glm/glm.hpp>
#include <string>
#include <array>
#include <tuple>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneView : EditorWindow<EDITOR_RESOURCES>
	{
		EW_SceneView ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& engineResources ) override;
	private:
		static constexpr unsigned int NO_TEXTURE = static_cast< unsigned int >( -1 );
	public:
		void SetRenderpassTexture ( unsigned int texture = NO_TEXTURE );
	private:
		unsigned int m_renderpass_texture { NO_TEXTURE };
		float m_margin_buffer_x { 20.0f };
		float m_margin_buffer_y { 100.0f };
		bool m_disable_guizmo { false };
		EnttXSol& m_enttxsol;

		std::array<std::tuple<std::string , ImGuizmo::OPERATION> , 3> m_guizmo_operation = {
			{{"Translate", ImGuizmo::OPERATION::TRANSLATE},
			{"Rotate", ImGuizmo::OPERATION::ROTATE},
			{"Scale", ImGuizmo::OPERATION::SCALE}} };
		uint32_t m_guizmo_operation_id { 0 };
	};
}

#include "EW_SceneTree.h"
#include "EW_TilemapEditor.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_SceneView<EDITOR_RESOURCES>::EW_SceneView ( EnttXSol& enttxsol )
		:
		m_enttxsol { enttxsol }
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneView<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& engineResources )
	{
		( dt );

		Camera& camera = engineResources.Get<Camera> ().get ();

		ImGui::Begin ( "Scene View" );
		if ( m_renderpass_texture != NO_TEXTURE )
		{
			auto window_size = ImVec2 ( ImGui::GetWindowWidth () - m_margin_buffer_x , ImGui::GetWindowHeight () - m_margin_buffer_y );
			ImVec2 viewport_pos { ImGui::GetWindowPos () } , viewport_size;
			if ( window_size.x > window_size.y * camera.m_aspect_ratio )
			{
				viewport_size = { window_size.y * camera.m_aspect_ratio, window_size.y };
				float cursor_x = ( ImGui::GetWindowWidth () - viewport_size.x ) / 2.0f;
				ImGui::SetCursorPosX ( cursor_x );
				viewport_pos.x += cursor_x;
			}
			else
			{
				viewport_size = { window_size.x, window_size.x * ( 1.0f / camera.m_aspect_ratio ) };
				float cursor_y = ( ImGui::GetWindowHeight () - viewport_size.y ) / 2.0f;
				ImGui::SetCursorPosY ( cursor_y );
				viewport_pos.y += cursor_y;
			}

			GLFWWindow& window = engineResources.Get<GLFWWindow> ().get ();

			auto viewport_x = ( ( window.ScreenMouseX () - ImGui::GetCursorScreenPos ().x ) / viewport_size.x ) * window.GetWindowWidth ();
			auto viewport_y = ( ( window.ScreenMouseY () - ImGui::GetCursorScreenPos ().y ) / viewport_size.y ) * window.GetWindowHeight ();

			window.SetViewportMouseCoordinates ( viewport_x , viewport_y );

			ImGui::Image ( ( void* ) ( static_cast< uint64_t >( m_renderpass_texture ) ) , viewport_size , { 0,1 } , { 1,0 } );

			// imguizmo code
			EnttXSol::Entities::ID selected_entity = this->Get<EW_SceneTree> ()->GetSelectedEntity ();
			if ( selected_entity != EnttXSol::Entities::Null )
			{
				if ( !m_disable_guizmo )
				{
					Transform* transform = m_enttxsol.GetEngineComponent<Transform> ( selected_entity );
					GridCell* grid_cell = m_enttxsol.GetEngineComponent<GridCell> ( selected_entity );
					if ( transform )
					{
						ImGuizmo::SetOrthographic ( false );
						ImGuizmo::SetDrawlist ();
						ImGuizmo::SetRect ( viewport_pos.x , viewport_pos.y , viewport_size.x , viewport_size.y );

						// camera
						glm::mat4 view = camera.GetCameraViewMatrix ();
						glm::mat4 projection = camera.GetPerpectiveProjectionMatrix ();

						// entity transform
						glm::mat4 world_transform = transform->m_parent_transform * transform->m_local_transform;

						ImGuizmo::Manipulate (
							glm::value_ptr ( view ) ,
							glm::value_ptr ( projection ) ,
							std::get<1> ( m_guizmo_operation[ m_guizmo_operation_id ] ) ,
							ImGuizmo::LOCAL ,
							glm::value_ptr ( world_transform ) );

						glm::mat4 local_transform = glm::inverse ( transform->m_parent_transform ) * world_transform;

						// decompose
						glm::vec3 position , rotation , scale;
						ImGuizmo::DecomposeMatrixToComponents (
							glm::value_ptr ( local_transform ) ,
							glm::value_ptr ( position ) , glm::value_ptr ( rotation ) , glm::value_ptr ( scale ) );

						if ( grid_cell )
						{
							grid_cell->m_cell_x = static_cast< int32_t >( std::floor ( position.x / ( grid_cell->m_cell_size * 2.0f ) ) );
							grid_cell->m_cell_y = static_cast< int32_t >( std::floor ( position.y / ( grid_cell->m_cell_size * 2.0f ) ) );
							grid_cell->m_cell_z = static_cast< int32_t >( std::floor ( position.z / ( grid_cell->m_cell_size * 2.0f ) ) );
							transform->m_rotation = rotation;
							transform->m_scale = scale;
						}
						else
						{
							transform->m_position = position;
							transform->m_rotation = rotation;
							transform->m_scale = scale;
						}
					}
				}
			}
		}
		if ( ImGui::Button ( "PLAY" ) )
		{
			m_enttxsol.m_pause = false;
			AudioAPI::ResumeAll();
		}
		this->ToolTipOnHover ( "Objects will be updated." );
		ImGui::SameLine ();
		if ( ImGui::Button ( "PAUSE" ) )
		{
			m_enttxsol.m_pause = true;
			AudioAPI::PauseAll();
		}
		this->ToolTipOnHover ( "Objects will not be updated." );
		ImGui::SameLine ();
		if ( ImGui::Button ( "RESET" ) )
		{
			this->Get<EW_SceneTree> ()->ResetScene ( engineResources );

			//// stopping and resetting all the sounds 
			//SoundManager& sound_manager = engineResources.Get<SoundManager>().get();
			//auto& sounds = sound_manager.GetResources();
			//AudioAPI::StopAndResetAll(sounds);
		}
		this->ToolTipOnHover ( "Resets the scene to its original state." );

		if ( ImGui::BeginPopup ( "Guizmo Operation" ) )
		{
			if ( ImGui::Selectable ( "Translate" ) )
			{
				m_guizmo_operation_id = 0;
				ImGui::CloseCurrentPopup ();
			}
			if ( ImGui::Selectable ( "Rotate" ) )
			{
				m_guizmo_operation_id = 1;
				ImGui::CloseCurrentPopup ();
			}
			if ( ImGui::Selectable ( "Scale" ) )
			{
				m_guizmo_operation_id = 2;
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}

		ImGui::SameLine ();
		if ( ImGui::Button ( ( std::string ( "Guizmo: " ) + std::get<0> ( m_guizmo_operation[ m_guizmo_operation_id ] ) ).c_str () ) )
		{
			ImGui::OpenPopup ( "Guizmo Operation" );
		}
		this->ToolTipOnHover ( "Changes the guizmo used to edit and object." );

		ImGui::SameLine ();
		ImGui::Checkbox ( "Disable Guizmo" , &m_disable_guizmo );

		ImGui::SameLine ();
		ImGui::Checkbox ( "Scene Camera" , &camera.m_free_camera_active );

		ImGui::SameLine ();
		if ( ImGui::Button ( "Reset Camera" ) )
		{
			camera.SetNextLookAt ( { 1,0,-2 } );
			camera.SetNextPosition ( { 8,10,2 } );
		}

		ImGui::SameLine ();
		ImGui::Text ( "FPS: %d" , DeltaTimer::m_fps );

		ImGui::SameLine ();
		auto tilemap_editor = this->Get<EW_TilemapEditor> ();
		ImGui::Text ( "Parent: %d, CellX: %d, CellZ: %d" , tilemap_editor->m_selected , tilemap_editor->m_cell_x , tilemap_editor->m_cell_z );

		PhysicsSystem& psystem = engineResources.Get<PhysicsSystem>().get();
		ImGui::Checkbox("Debug Shapes", &psystem.debugdraw);


		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneView<EDITOR_RESOURCES>::SetRenderpassTexture ( unsigned int texture )
	{
		m_renderpass_texture = texture;
	}
}