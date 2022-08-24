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
		uint32_t m_selected_scene { 0 };

		std::vector<std::string> m_prefabs_list;
		std::vector<std::string> m_scene_list;

		void RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity );

		void DisplayPrefabsPopup ( EDITOR_RESOURCES& engineResources );
		void OpenPrefabPopup ();

		void RefreshSceneList ();
	};
}

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
		ImGui::Begin ( "Scene Tree" );

		if ( ImGui::BeginTabBar ( "Scene" ) )
		{
			if ( ImGui::BeginTabItem ( "Hierarchy" ) )
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
				if ( ImGui::Button ( "Save Scene" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					ImGui::OpenPopup ( "SerializeScene" );
				}
				this->ToolTipOnHover ( "Saves the current scene." );

				// adding to root
				if ( ImGui::Button ( "Add Object" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					ImGui::OpenPopup ( "Create Entity" );
				}
				this->ToolTipOnHover ( "Adds a new object to the scene." );

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
					m_enttxsol.RemoveEntity ( m_selected_remove );
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
					Reset ();
					m_enttxsol.Clear ();
				}
				this->ToolTipOnHover ( "Clears the current scene, providing a fresh canvas." );

				// display all scenes as selectable
				uint32_t i { 0 };
				for ( auto const& scene : m_scene_list )
				{
					auto last_dash = scene.find_last_of ( '/' );
					auto last_dot = scene.find_last_of ( '.' );
					std::string name = scene.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
					ImGui::Selectable ( name.c_str () , m_selected_scene == i );
					if ( ImGui::BeginPopupContextItem () )
					{
						m_selected_scene = i;
						if ( ImGui::Selectable ( "Load" ) )
						{
							Reset ();
							m_enttxsol.Clear ();
							m_enttxsol.DeserializeStateV2 ( engineResources , name );
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
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity )
	{
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
		if ( ImGui::IsItemClicked () )
		{
			m_selected_entity = entity;
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
			if ( ImGui::BeginListBox ( "##PrefabsList" ) )
			{
				for ( auto const& prefab : m_prefabs_list )
				{
					auto last_dash = prefab.find_last_of ( '/' );
					auto last_dot = prefab.find_last_of ( '.' );
					std::string name = prefab.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
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
		if ( m_selected_scene < static_cast< uint32_t >( m_scene_list.size () ) )
		{
			Reset ();
			m_enttxsol.Clear ();
			std::string scene_path = m_scene_list[ m_selected_scene ];
			auto last_dash = scene_path.find_last_of ( '/' );
			auto last_dot = scene_path.find_last_of ( '.' );
			std::string name = scene_path.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
			m_enttxsol.DeserializeStateV2 ( engineResources , name );
		}
	}
}