#pragma once
#include "../Editor.h"

#include "../../ECS/ECSManager.h"
#include "../../Window/GLFWWindow.h"

#include <string>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneManager : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	private:
		std::string m_new_entity_name;

		static constexpr uint32_t UNSELECTED = static_cast< uint32_t >( -1 );
		uint32_t m_selected_entity { UNSELECTED };
	};
}

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void EW_SceneManager<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		ImGui::Begin ( "Scene Manager" );

		// tree heirarchy of all entities in the scene
		ECSManager& ecs_manager = editorResources.Get<ECSManager> ().get ();

		if ( ImGui::BeginTabBar ( "SceneManagerTabBar" ) )
		{
			if ( ImGui::BeginTabItem ( "Scene" ) )
			{
				// display all entities
				EntityCollection const& entities = ecs_manager.m_ecs.GetEntities ();

				ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				for ( size_t i = 0; i < entities.size (); ++i )
				{
					if ( entities[ i ].has_value () )
					{
						Entity const& entity = entities[ i ].value ();
						ImGui::PushID ( i );
						ImGui::SetNextItemOpen ( true , ImGuiCond_Once );
						if ( ImGui::TreeNodeEx ( entity.m_name.c_str () ,
							m_selected_entity == i ? tree_node_flags | ImGuiTreeNodeFlags_Selected : tree_node_flags ) )
						{
							if ( ImGui::IsItemClicked () )
							{
								m_selected_entity = i;
							}
							ImGui::TreePop ();
						}
						ImGui::PopID ();
					}
				}

				if ( ImGui::BeginPopup ( "NewEntityName" ) )
				{
					ImGui::InputText ( ":Name" , &m_new_entity_name );
					if ( ImGui::Button ( "Add" ) || editorResources.Get<GLFWWindow> ().get ().KeyPressed ( GLFW_KEY_ENTER ) )
					{
						ecs_manager.m_ecs.AddEntity ( m_new_entity_name );
						m_new_entity_name = "";
						ImGui::CloseCurrentPopup ();
					}
					ImGui::EndPopup ();
				}

				if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth (), 0.0f } ) )
				{
					ImGui::OpenPopup ( "NewEntityName" );
				}

				if ( ImGui::Button ( "Close" , { ImGui::GetWindowWidth (), 0.0f } ) )
				{
					this->Close ();
				}

				ImGui::EndTabItem ();
			}

			// display selected entity components
			if ( ImGui::BeginTabItem ( "Selected Entity" ) )
			{
				if ( m_selected_entity != UNSELECTED )
				{
					ImGui::Text ( "%d entity" , m_selected_entity );
				}
				else
				{
					ImGui::Text ( "No entity selected." );
				}

				ImGui::EndTabItem ();
			}
			ImGui::EndTabBar ();
		}

		ImGui::End ();
	}
}