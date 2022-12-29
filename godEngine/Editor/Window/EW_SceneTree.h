#pragma once
#include "../Editor.h"
#include "../../EnttXSol/EnttXSol.h"
#include "../../imgui/imgui_stdlib.h"

#include <godUtility/Internal/FolderHelper.h>

#include <string>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneTree : EditorWindow<EDITOR_RESOURCES>
	{
		EW_SceneTree ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& engineResources ) override;
		void Reset ();
		EnttXSol::Entities::ID GetSelectedEntity ();
		void ResetScene ( EDITOR_RESOURCES& engineResources );
		void SetSelectedScene ( std::string const& name );
	private:
		std::string m_input_string { "NIL" };
		EnttXSol::Entities::ID m_selected_entity { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_parent { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_parent_temp { m_selected_parent };
		EnttXSol::Entities::ID m_selected_prefab_parent { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_prefab_parent_temp { m_selected_prefab_parent };
		EnttXSol::Entities::ID m_selected_remove { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_new_prefab { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_new_prefab_temp { m_selected_new_prefab };

		EnttXSol& m_enttxsol;

		int m_uid { 0 };
		int m_selected_scene { -1 };

		std::vector<std::string> m_prefabs_list;
		std::vector<std::string> m_scene_list;

	public:
		bool m_select_hierarchy_tab { false };
	private:

		void RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity );

		void DisplayPrefabsPopup ( EDITOR_RESOURCES& engineResources );
		void OpenPrefabPopup ();

		void RefreshSceneList ();
	};
}

#include "EW_EntityEditor.h"
#include "EW_TilemapEditor.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_SceneTree<EDITOR_RESOURCES>::EW_SceneTree ( EnttXSol& enttxsol )
		:
		m_enttxsol { enttxsol }
	{
		RefreshSceneList ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& engineResources )
	{
		( dt );

		GLFWWindow& glwind = engineResources.Get< GLFWWindow> ().get ();
		PhysicsSystem& psys = engineResources.Get< PhysicsSystem> ().get ();

		if ( glwind.KeyDown ( GLFW_KEY_LEFT_CONTROL ) && glwind.MouseLPressed () )
		{
			m_selected_entity = psys.getRCMid ();
		}

		/*		if (glwind.KeyPressed(GLFW_KEY_P))
					m_selected_entity = psys.getRCMid();*/



		ImGui::Begin ( "Scene Tree" );

		if ( ImGui::BeginTabBar ( "Scene" ) )
		{
			ImGuiTabItemFlags flag { 0 };
			if ( m_select_hierarchy_tab )
			{
				flag |= ImGuiTabItemFlags_SetSelected;
				m_select_hierarchy_tab = false;
			}
			if ( ImGui::BeginTabItem ( "Hierarchy" , 0 , flag ) )
			{
				// Creating entity without parent
				if ( ImGui::BeginPopup ( "Create Entity" ) )
				{
					ImGui::InputText ( "##EntityName" , &m_input_string );
					if ( ImGui::Button ( "Create With Name" ) )
					{
						m_enttxsol.CreateEntity ( m_input_string );
						m_input_string = { "NIL" };
						ImGui::CloseCurrentPopup ();
					}
					ImGui::EndPopup ();
				}

				DisplayPrefabsPopup ( engineResources );

				if ( ImGui::BeginPopup ( "Add Entity" ) )
				{
					ImGui::InputText ( "##EntityName" , &m_input_string );
					if ( ImGui::Button ( "Create With Name" ) )
					{
						m_enttxsol.CreateEntity ( m_input_string , m_selected_parent_temp );
						m_input_string = "NIL";
						m_selected_parent_temp = EnttXSol::Entities::Null;
						ImGui::CloseCurrentPopup ();
					}
					ImGui::EndPopup ();
				}

				if ( ImGui::BeginPopup ( "Save As" ) )
				{
					ImGui::InputText ( "##PrefabName" , &m_input_string );
					if ( ImGui::Button ( "Save As" ) )
					{
						m_enttxsol.SavePrefabV2 ( engineResources , m_selected_new_prefab_temp , m_input_string );
						m_input_string = "NIL";
						m_selected_new_prefab_temp = EnttXSol::Entities::Null;
						ImGui::CloseCurrentPopup ();
					}
					ImGui::EndPopup ();
				}

				// serialize scene
				if ( ImGui::BeginPopup ( "SerializeScene" ) )
				{
					ImGui::InputText ( "##SceneName" , &m_input_string );
					if ( ImGui::Button ( "Create With Name" ) )
					{
						m_enttxsol.SerializeStateV2 ( engineResources , m_input_string );
						RefreshSceneList ();
						m_input_string = "NIL";
						ImGui::CloseCurrentPopup ();
					}
					ImGui::EndPopup ();
				}
				if ( m_selected_scene < m_scene_list.size () && m_selected_scene >= 0 )
				{
					std::string scene_path = m_scene_list[ m_selected_scene ];
					auto last_dash = scene_path.find_last_of ( '/' );
					auto last_dot = scene_path.find_last_of ( '.' );
					std::string name = scene_path.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
					ImGui::Text ( "[%s]" , name.c_str () );
				}
				else
				{
					ImGui::Text ( "[NONE]" );
				}
				ImGui::Separator ();
				if ( ImGui::Button ( "Save Scene" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					if ( m_selected_scene < m_scene_list.size () && m_selected_scene >= 0 )
					{
						std::string scene_path = m_scene_list[ m_selected_scene ];
						auto last_dash = scene_path.find_last_of ( '/' );
						auto last_dot = scene_path.find_last_of ( '.' );
						std::string name = scene_path.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
						m_input_string = name;
					}

					ImGui::OpenPopup ( "SerializeScene" );
				}
				this->ToolTipOnHover ( "Saves the current scene." );

				// adding to root
				if ( ImGui::Button ( "Add Object" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					ImGui::OpenPopup ( "Create Entity" );
				}
				this->ToolTipOnHover ( "Adds a new object to the scene." );

				if ( ImGui::Button ( "Add Prefab" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					OpenPrefabPopup ();
					m_selected_prefab_parent_temp = EnttXSol::Entities::Null;
				}
				this->ToolTipOnHover ( "Adds a prefab to the scene." );

				// adding object
				if ( m_selected_parent != EnttXSol::Entities::Null )
				{
					ImGui::OpenPopup ( "Add Entity" );
					m_selected_parent_temp = m_selected_parent;
					m_selected_parent = EnttXSol::Entities::Null;
				}

				// adding prefab
				if ( m_selected_prefab_parent != EnttXSol::Entities::Null )
				{
					OpenPrefabPopup ();
					m_selected_prefab_parent_temp = m_selected_prefab_parent;
					m_selected_prefab_parent = EnttXSol::Entities::Null;
				}

				// prefabing
				if ( m_selected_new_prefab != EnttXSol::Entities::Null )
				{
					ImGui::OpenPopup ( "Save As" );
					m_selected_new_prefab_temp = m_selected_new_prefab;
					m_selected_new_prefab = EnttXSol::Entities::Null;
				}

				// removing entity
				if ( m_selected_remove != EnttXSol::Entities::Null )
				{
					// if entity if part of grid
					m_enttxsol.RemoveEntity ( engineResources.Get<EntityGrid> ().get () , m_selected_remove );
					/*GridCell* grid_cell = m_enttxsol.GetEngineComponent<GridCell> ( m_selected_remove );
					if ( grid_cell )
					{
						m_enttxsol.RemoveEntityFromGrid ( engineResources.Get<EntityGrid> ().get () , m_selected_remove );
					}
					else
					{
						m_enttxsol.RemoveEntity ( m_selected_remove );
					}*/
					m_selected_entity = EnttXSol::Entities::Null;
					m_selected_remove = EnttXSol::Entities::Null;
				}

				// render scene hierarchy
				for ( uint32_t i = 0; i < m_enttxsol.m_entities.Size (); ++i )
				{
					// call function only on root nodes
					if ( m_enttxsol.m_entities.Valid ( i ) && m_enttxsol.m_entities[ i ].m_parent_id == EnttXSol::Entities::Null )
					{
						RecursivelyDisplaySceneHierarchy ( engineResources , i );
					}
				}

				m_uid = 0;
				ImGui::EndTabItem ();
			}
			if ( ImGui::BeginTabItem ( "Scenes" ) )
			{
				if ( ImGui::Button ( "New Scene" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					this->Get<EW_TilemapEditor> ()->ClearPreview ( engineResources );
					Reset ();
					m_enttxsol.ClearEntt ( engineResources );
					m_selected_scene = -1;
					m_select_hierarchy_tab = true;
					engineResources.Get<EntityGrid> ().get () = EntityGrid ();

					// stopping and resetting all the sounds 
					SoundManager& sound_manager = engineResources.Get<SoundManager> ().get ();
					auto& sounds = sound_manager.GetResources ();
					AudioAPI::StopAndResetAll ( sounds );

				}
				this->ToolTipOnHover ( "Clears the current scene, providing a fresh canvas." );

				// display all scenes as selectable
				int i { 0 };
				for ( auto const& scene : m_scene_list )
				{
					auto last_dash = scene.find_last_of ( '/' );
					auto last_dot = scene.find_last_of ( '.' );
					std::string name = scene.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
					ImGui::Selectable ( name.c_str () , m_selected_scene == i );
					this->ToolTipOnHover ( "Right Click for Options." );
					if ( ImGui::BeginPopupContextItem () )
					{
						m_selected_scene = i;
						if ( ImGui::Selectable ( "Load" ) )
						{
							this->Get<EW_TilemapEditor> ()->ClearPreview ( engineResources );
							EntityGrid& grid = engineResources.Get<EntityGrid> ().get (); // clear tilemap grid
							grid = EntityGrid ();
							Reset ();
							m_enttxsol.ClearEntt ( engineResources );
							m_enttxsol.DeserializeStateV2 ( engineResources , name , &grid );
							m_select_hierarchy_tab = true;

							// stopping and resetting all the sounds 
							SoundManager& sound_manager = engineResources.Get<SoundManager> ().get ();
							auto& sounds = sound_manager.GetResources ();
							AudioAPI::StopAndResetAll ( sounds );
						}
						this->ToolTipOnHover ( "Loads the scene up as the current scene." );

						ImGui::EndPopup ();
					}
					++i;
				}

				ImGui::EndTabItem ();
			}
			ImGui::EndTabBar ();
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::Reset ()
	{
		m_input_string = "NIL";
		m_selected_entity = EnttXSol::Entities::Null;
		m_selected_parent = EnttXSol::Entities::Null;
		m_selected_parent_temp = m_selected_parent;
		m_selected_prefab_parent = EnttXSol::Entities::Null;
		m_selected_prefab_parent_temp = m_selected_prefab_parent;
		m_selected_remove = EnttXSol::Entities::Null;
		m_selected_new_prefab = EnttXSol::Entities::Null;
		m_selected_new_prefab_temp = m_selected_new_prefab;
	}

	template<typename EDITOR_RESOURCES>
	inline EnttXSol::Entities::ID EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		if ( !m_enttxsol.m_entities.Valid ( m_selected_entity ) )
		{
			m_selected_entity = EnttXSol::Entities::Null;
			return m_selected_entity;
		}
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity )
	{
		if ( !m_enttxsol.m_entities[ entity ].m_persist_in_scene )
		{
			return;
		}
		ImGui::PushID ( m_uid++ );
		bool is_prefab = m_enttxsol.m_entities[ entity ].m_type == Entity_::Type::Prefab;
		ImGuiTreeNodeFlags tree_node_flags { 0 };
		std::string node_name { "" };
		if ( is_prefab )
		{
			if ( m_enttxsol.m_entities[ entity ].m_children.size () > 0 )
			{
				tree_node_flags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			}
			else
			{
				tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
			( node_name += "[p] " ) += m_enttxsol.m_entities[ entity ].m_name.c_str ();
		}
		else
		{
			if ( m_enttxsol.m_entities[ entity ].m_children.size () > 0 )
			{
				tree_node_flags |= ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			}
			else
			{
				tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
			}
			node_name = m_enttxsol.m_entities[ entity ].m_name;
		}
		if ( m_selected_entity == entity )
		{
			tree_node_flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool open = ImGui::TreeNodeEx ( node_name.c_str () , tree_node_flags );
		this->ToolTipOnHover ( "Left-Click: Select/Unselect.\nRight-Click: Options." );
		if ( ImGui::IsItemClicked () )
		{
			if ( m_selected_entity == entity )
			{
				m_selected_entity = EnttXSol::Entities::Null;
			}
			else
			{
				m_selected_entity = entity;
			}

			this->Get<EW_EntityEditor> ()->m_select_inspector_tab = true;

		}
		if ( is_prefab )
		{
			if ( ImGui::BeginPopupContextItem ( "onEntityRightClick" ) )
			{
				m_selected_entity = entity;
				if ( ImGui::Selectable ( "Add Object" ) )
				{
					m_selected_parent = entity;
				}
				this->ToolTipOnHover ( "Add a new object as child of this object." );
				if ( ImGui::Selectable ( "Add Prefab" ) )
				{
					m_selected_prefab_parent = entity;
					//m_enttxsol.LoadPrefabV2 ( engineResources , "prefabv3" , entity );
				}
				this->ToolTipOnHover ( "Add a prefab as child of this object." );
				if ( ImGui::Selectable ( "Rename" ) )
				{

				}
				this->ToolTipOnHover ( "Rename this entity. // not implemented yet!" );
				if ( ImGui::Selectable ( "Save" ) )
				{
					m_enttxsol.SavePrefabV2 ( engineResources , entity , m_enttxsol.m_entities[ entity ].m_name );
				}
				this->ToolTipOnHover ( "Updates this prefab in file with it's current state." );
				if ( ImGui::Selectable ( "Save As" ) )
				{
					m_selected_new_prefab = entity;
				}
				this->ToolTipOnHover ( "Create a new prefab with this object as the root." );
				if ( ImGui::Selectable ( "Remove" ) )
				{
					m_selected_remove = entity;
				}
				this->ToolTipOnHover ( "Remove this object and all its children from the scene." );
				ImGui::EndPopup ();
			}
		}
		else
		{
			if ( ImGui::BeginPopupContextItem ( "onEntityRightClick" ) )
			{
				m_selected_entity = entity;
				if ( ImGui::Selectable ( "Add Object" ) )
				{
					m_selected_parent = entity;
				}
				this->ToolTipOnHover ( "Add a new object as child of this object." );
				if ( ImGui::Selectable ( "Add Prefab" ) )
				{
					m_selected_prefab_parent = entity;
					//m_enttxsol.LoadPrefabV2 ( engineResources , "prefabv3" , entity );
				}
				this->ToolTipOnHover ( "Add a prefab as child of this object." );
				if ( ImGui::Selectable ( "Save As" ) )
				{
					m_selected_new_prefab = entity;
				}
				this->ToolTipOnHover ( "Create a new prefab with this object as the root." );
				if ( ImGui::Selectable ( "Remove" ) )
				{
					m_selected_remove = entity;
				}
				this->ToolTipOnHover ( "Remove this object and all its children from the scene." );
				ImGui::EndPopup ();
			}
		}

		// display children entities
		if ( open )
		{
			// Creating parent as child of entity
			for ( auto const& child : m_enttxsol.m_entities[ entity ].m_children )
			{
				RecursivelyDisplaySceneHierarchy ( engineResources , child );
			}
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::DisplayPrefabsPopup ( EDITOR_RESOURCES& engineResources )
	{
		if ( ImGui::BeginPopup ( "PrefabsPopup" ) )
		{
			ImGui::Text ( "Choose A Prefab :" );
			ImGui::Separator ();
			if ( ImGui::BeginListBox ( "##PrefabsList" , ImVec2 ( 0 , 400.0f ) ) )
			{
				char first_alphabet { 'z' };
				for ( auto const& prefab : m_prefabs_list )
				{
					auto last_dash = prefab.find_last_of ( '/' );
					auto last_dot = prefab.find_last_of ( '.' );
					std::string name = prefab.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );

					if ( std::toupper ( name[ 0 ] ) != first_alphabet )
					{
						first_alphabet = std::toupper ( name[ 0 ] );
						ImGui::Separator ();
						ImGui::SameLine ( ImGui::GetWindowWidth () - 50.0f );
						ImGui::Text ( "[%c]" , first_alphabet );
						ImGui::Separator ();
					}

					if ( ImGui::Selectable ( name.c_str () ) )
					{
						m_enttxsol.LoadPrefabV2 ( engineResources , name , m_selected_prefab_parent_temp );
						ImGui::CloseCurrentPopup ();
					}
				}
				ImGui::EndListBox ();
			}

			ImGui::EndPopup ();
		}
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::OpenPrefabPopup ()
	{
		// read all prefabs from folder
		m_prefabs_list = FolderHelper::GetFiles ( "Assets/GameAssets/Prefabs/" );

		ImGui::OpenPopup ( "PrefabsPopup" );
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RefreshSceneList ()
	{
		m_scene_list = FolderHelper::GetFiles ( "Assets/GameAssets/Scenes/" );
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::ResetScene ( EDITOR_RESOURCES& engineResources )
	{
		if ( m_selected_scene < static_cast< int >( m_scene_list.size () ) )
		{
			this->Get<EW_TilemapEditor> ()->ClearPreview ( engineResources );
			Reset ();
			m_enttxsol.ClearEntt ( engineResources );
			engineResources.Get<EntityGrid> ().get () = EntityGrid ();
			if ( m_selected_scene < m_scene_list.size () && m_selected_scene >= 0 )
			{
				std::string scene_path = m_scene_list[ m_selected_scene ];
				auto last_dash = scene_path.find_last_of ( '/' );
				auto last_dot = scene_path.find_last_of ( '.' );
				std::string name = scene_path.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
				// reset grid
				EntityGrid& grid = engineResources.Get<EntityGrid> ().get (); // clear tilemap grid
				grid = EntityGrid ();

				// reset physics controllers is any
				PX::PhysicsController& physics_controller = engineResources.Get<PX::PhysicsController> ().get ();
				physics_controller.FreeControllers ();

				// reload scene
				m_enttxsol.DeserializeStateV2 ( engineResources , name );

				// stopping and resetting all the sounds 
				SoundManager& sound_manager = engineResources.Get<SoundManager> ().get ();
				auto& sounds = sound_manager.GetResources ();
				AudioAPI::StopAndResetAll ( sounds );
			}
		}
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::SetSelectedScene ( std::string const& name )
	{
		auto it = std::find_if ( m_scene_list.begin () , m_scene_list.end () , [&name]( auto const& s )
			{
				return ( s.find ( name ) != std::string::npos );
			} );
		if ( it != m_scene_list.end () )
		{
			m_selected_scene = static_cast< int >( it - m_scene_list.begin () );
		}
	}
}