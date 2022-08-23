#pragma once
#include "../Editor.h"
#include "../../EnttXSol/EnttXSol.h"

#include "../../imgui/imgui_stdlib.h" 

#include <string>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneTree : EditorWindow<EDITOR_RESOURCES>
	{
		EW_SceneTree ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
		EnttXSol::Entities::ID GetSelectedEntity ();
	private:
		std::string m_input_string { "NIL" };
		EnttXSol::Entities::ID m_selected_entity { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_parent { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_parent_temp { m_selected_parent };
		EnttXSol::Entities::ID m_selected_remove { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_child_index { EnttXSol::Entities::Null }; // index representing position in the children container of its parent
		EnttXSol::Entities::ID m_selected_new_prefab { EnttXSol::Entities::Null };
		EnttXSol::Entities::ID m_selected_new_prefab_temp { m_selected_new_prefab };
		EnttXSol::Entities::ID m_remove_prefab { EnttXSol::Entities::Null };
		bool m_save_prefab { false };
		EnttXSol& m_enttxsol;

		int m_uid { 0 };

		void RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity /*, std::vector<EnttXSol::EntityData> const& entityData , std::vector<EnttXSol::Prefab> const& prefabs , uint32_t childIndex = EnttXSol::Entities::Null*/ );

		void ToolTipOnHover ( std::string const& text );
		//void DisplayPrefab ( EnttXSol::Prefab const& prefab , uint32_t id = -1 );
	};
}

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_SceneTree<EDITOR_RESOURCES>::EW_SceneTree ( EnttXSol& enttxsol )
		:
		m_enttxsol { enttxsol }
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		( dt );
		//auto& entities = m_enttxsol.GetEntities ();
		//auto& entity_data = m_enttxsol.GetEntityData ();
		//auto const& prefabs = m_enttxsol.GetPrefabs ();

		ImGui::Begin ( "Scene Tree" );
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
				m_enttxsol.SavePrefabV2 ( editorResources , m_selected_new_prefab_temp , m_input_string );
				m_input_string = "NIL";
				m_selected_new_prefab_temp = EnttXSol::Entities::Null;
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		/*if ( ImGui::BeginPopup ( "Save Prefab" ) )
		{
			ImGui::InputText ( "##SaveName" , &m_input_string );
			if ( ImGui::Button ( "Save As" ) )
			{
				m_enttxsol.SavePrefab ( editorResources , m_selected_new_prefab_temp , std::string ( "Assets/GameAssets/Prefabs/" ) + m_input_string + ".json" , true );
				m_input_string = "NIL";
				m_selected_new_prefab_temp = EnttXSol::Entities::Null;
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}*/
		// adding to root
		if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			ImGui::OpenPopup ( "Create Entity" );
		}
		// adding child
		if ( m_selected_parent != EnttXSol::Entities::Null )
		{
			ImGui::OpenPopup ( "Add Entity" );
			m_selected_parent_temp = m_selected_parent;
			m_selected_parent = EnttXSol::Entities::Null;
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
			m_selected_child_index = EnttXSol::Entities::Null;
		}
		// removing prefab
		/*if ( m_remove_prefab != EnttXSol::Entities::Null )
		{

		}*/
		// display prefabs
		/*auto prefab_count { 0 };
		for ( auto const& prefab : prefabs )
		{
			if ( std::get<1> ( std::get<1> ( prefab )[ 0 ] ).m_parent == EnttXSol::Entities::Null )
			{
				DisplayPrefab ( prefab , prefab_count );
			}
			++prefab_count;
		}*/
		// render scene hierarchy
		for ( auto i = 0; i < m_enttxsol.m_entities.Size (); ++i )
		{
			// call function only on root nodes
			if ( m_enttxsol.m_entities.Valid ( i ) && m_enttxsol.m_entities[ i ].m_parent_id == EnttXSol::Entities::Null )
			{
				RecursivelyDisplaySceneHierarchy ( editorResources , i /*, entity_data , prefabs*/ );
			}
		}

		// test serialize
		if ( ImGui::Button ( "Serialize" ) )
		{
			m_enttxsol.SerializeStateV2 ( editorResources , "SerializeV2" );
		}
		ImGui::End ();

		m_uid = 0;
	}

	template<typename EDITOR_RESOURCES>
	inline EnttXSol::Entities::ID EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , EnttXSol::Entities::ID entity /*, std::vector<EnttXSol::EntityData> const& entityData , std::vector<EnttXSol::Prefab> const& prefabs , uint32_t childIndex*/ )
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
				ToolTipOnHover ( "Add a new object as child of this object." );
				if ( ImGui::Selectable ( "Add Prefab" ) )
				{
					m_enttxsol.LoadPrefabV2 ( engineResources , "prefabv3" , entity );
				}
				ToolTipOnHover ( "Add a prefab as child of this object." );
				if ( ImGui::Selectable ( "Save" ) )
				{
					m_enttxsol.SavePrefabV2 ( engineResources , entity , m_enttxsol.m_entities[ entity ].m_name );
				}
				ToolTipOnHover ( "Updates this prefab in file with it's current state." );
				if ( ImGui::Selectable ( "Save As" ) )
				{
					m_selected_new_prefab = entity;
				}
				ToolTipOnHover ( "Create a new prefab with this object as the root." );
				if ( ImGui::Selectable ( "Remove" ) )
				{
					m_selected_remove = entity;
				}
				ToolTipOnHover ( "Remove this object and all its children from the scene." );
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
				ToolTipOnHover ( "Add a new object as child of this object." );
				if ( ImGui::Selectable ( "Add Prefab" ) )
				{
					m_enttxsol.LoadPrefabV2 ( engineResources , "prefabv3" , entity );
				}
				ToolTipOnHover ( "Add a prefab as child of this object." );
				if ( ImGui::Selectable ( "Save As" ) )
				{
					m_selected_new_prefab = entity;
				}
				ToolTipOnHover ( "Create a new prefab with this object as the root." );
				if ( ImGui::Selectable ( "Remove" ) )
				{
					m_selected_remove = entity;
				}
				ToolTipOnHover ( "Remove this object and all its children from the scene." );
				ImGui::EndPopup ();
			}
		}

		// display children entities
		if ( open )
		{
			// display children prefabs
			//for ( auto const& child_prefab : data.m_prefab_children )
			//{
			//	/*ImGui::PushID ( m_uid++ );
			//	if ( ImGui::TreeNodeEx ( ( std::string ( "[p] " ) + std::get<1> ( std::get<1> ( prefabs[ child_prefab ] )[ 0 ] ).m_name ).c_str () , ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet ) )
			//	{
			//		ImGui::TreePop ();
			//	}
			//	ImGui::PopID ();*/
			//	DisplayPrefab ( prefabs[ child_prefab ] , child_prefab );
			//}
			//if ( !is_prefab )
			//{
			//	// Creating parent as child of entity
			//	//for ( auto i = 0; i < m_enttxsol.m_entities[ entity ].m_children.size (); ++i )
			//	//{
			//	//	RecursivelyDisplaySceneHierarchy ( engineResources , m_enttxsol.m_entities[ entity ].m_children[ i ] /*, entityData , prefabs , i*/ );
			//	//}
			//	for ( auto const& child : m_enttxsol.m_entities[ entity ].m_children )
			//	{
			//		RecursivelyDisplaySceneHierarchy ( engineResources , child );
			//	}
			//}
			//else
			{
				// Creating parent as child of entity
				for ( auto const& child : m_enttxsol.m_entities[ entity ].m_children )
				{
					/*if ( m_enttxsol.m_entities[ child ].m_type != Entity_::Type::Prefab || m_enttxsol.m_entities[ child ].m_root )
					{
						RecursivelyDisplaySceneHierarchy ( engineResources , child );
					}*/
					RecursivelyDisplaySceneHierarchy ( engineResources , child );
				}
			}
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::ToolTipOnHover ( std::string const& text )
	{
		if ( ImGui::IsItemHovered () )
		{
			ImGui::BeginTooltip ();
			ImGui::PushTextWrapPos ( ImGui::GetFontSize () * 35.0f );
			ImGui::TextUnformatted ( text.c_str () );
			ImGui::PopTextWrapPos ();
			ImGui::EndTooltip ();
		}
	}

	/*template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::DisplayPrefab ( EnttXSol::Prefab const& prefab , uint32_t id )
	{
		ImGui::PushID ( m_uid++ );
		if ( ImGui::TreeNodeEx ( ( std::string ( "[p] " ) + std::get<1> ( std::get<1> ( prefab )[ 0 ] ).m_name ).c_str () , ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet ) )
		{
			if ( ImGui::BeginPopupContextItem ( "onPrefabRightClick" ) )
			{
				if ( ImGui::Selectable ( "Remove" ) )
				{
					m_remove_prefab = id;
				}
				ImGui::EndPopup ();
			}

			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}*/
}