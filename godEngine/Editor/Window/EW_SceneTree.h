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
		Entity GetSelectedEntity ();
	private:
		std::string m_input_string { "NIL" };
		Entity m_selected_entity { EnttXSol::NullEntity };
		Entity m_selected_parent { EnttXSol::NullEntity };
		Entity m_selected_parent_temp { m_selected_parent };
		Entity m_selected_remove { EnttXSol::NullEntity };
		Entity m_selected_child_index { EnttXSol::NullEntity }; // index representing position in the children container of its parent
		Entity m_selected_prefab { EnttXSol::NullEntity };
		Entity m_selected_prefab_temp { m_selected_prefab };

		EnttXSol& m_enttxsol;

		int m_uid { 0 };

		void RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , Entity entity , std::vector<EnttXSol::EntityData> const& entityData , std::vector<EnttXSol::Prefab> const& prefabs , uint32_t childIndex = EnttXSol::NullEntity );

		void DisplayPrefab ( EnttXSol::Prefab const& prefab );
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
		auto& entities = m_enttxsol.GetEntities ();
		auto& entity_data = m_enttxsol.GetEntityData ();
		auto const& prefabs = m_enttxsol.GetPrefabs ();

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
		if ( ImGui::BeginPopup ( "Create Child Entity" ) )
		{
			ImGui::InputText ( "##EntityName" , &m_input_string );
			if ( ImGui::Button ( "Create With Name" ) )
			{
				m_enttxsol.CreateEntity ( m_input_string , m_selected_parent_temp );
				m_input_string = "NIL";
				m_selected_parent_temp = EnttXSol::NullEntity;
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		if ( ImGui::BeginPopup ( "Save As Prefab" ) )
		{
			ImGui::InputText ( "##PrefabName" , &m_input_string );
			if ( ImGui::Button ( "Save As" ) )
			{
				m_enttxsol.SavePrefab ( editorResources , m_selected_prefab_temp , std::string ( "Assets/GameAssets/Prefabs/" ) + m_input_string + ".json" );
				m_input_string = "NIL";
				m_selected_prefab_temp = EnttXSol::NullEntity;
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		// adding to root
		if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			ImGui::OpenPopup ( "Create Entity" );
		}
		// adding child
		if ( m_selected_parent != EnttXSol::NullEntity )
		{
			ImGui::OpenPopup ( "Create Child Entity" );
			m_selected_parent_temp = m_selected_parent;
			m_selected_parent = EnttXSol::NullEntity;
		}
		// prefabing
		if ( m_selected_prefab != EnttXSol::NullEntity )
		{
			ImGui::OpenPopup ( "Save As Prefab" );
			m_selected_prefab_temp = m_selected_prefab;
			m_selected_prefab = EnttXSol::NullEntity;
		}
		// removing entity
		if ( m_selected_remove != EnttXSol::NullEntity )
		{
			m_enttxsol.RemoveEntity ( m_selected_remove , m_selected_child_index );
			m_selected_entity = EnttXSol::NullEntity;
			m_selected_remove = EnttXSol::NullEntity;
			m_selected_child_index = EnttXSol::NullEntity;
		}
		// display prefabs
		for ( auto const& prefab : prefabs )
		{
			if ( std::get<1> ( std::get<1> ( prefab )[ 0 ] ).m_parent == EnttXSol::NullEntity )
			{
				DisplayPrefab ( prefab );
			}
		}
		// render scene hierarchy
		for ( auto i = 0; i < entities.size (); ++i )
		{
			// call function only on root nodes
			if ( entities[ i ].has_value () && entity_data[ i ].m_parent == EnttXSol::NullEntity )
			{
				RecursivelyDisplaySceneHierarchy ( editorResources , i , entity_data , prefabs );
			}
		}

		// test serialize
		if ( ImGui::Button ( "Serialize" ) )
		{
			m_enttxsol.SerializeState ( editorResources , "test3.json" );
		}
		ImGui::End ();

		m_uid = 0;
	}

	template<typename EDITOR_RESOURCES>
	inline Entity EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( EDITOR_RESOURCES& engineResources , Entity entity , std::vector<EnttXSol::EntityData> const& entityData , std::vector<EnttXSol::Prefab> const& prefabs , uint32_t childIndex )
	{
		ImGui::PushID ( m_uid++ );
		auto& data = entityData[ entity ];
		auto tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if ( m_selected_entity == entity )
		{
			tree_node_flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool open = ImGui::TreeNodeEx ( data.m_name.c_str () , tree_node_flags );
		if ( ImGui::IsItemClicked () )
		{
			m_selected_entity = entity;
		}
		if ( ImGui::BeginPopupContextItem ( "onEntityRightClick" ) )
		{
			if ( ImGui::Selectable ( "Add Child Entity" ) )
			{
				m_selected_parent = entity;
			}
			if ( ImGui::Selectable ( "Add Child Prefab" ) )
			{
				m_enttxsol.LoadPrefab ( engineResources , "NIL" , entity );
			}
			if ( ImGui::Selectable ( "Save As Prefab" ) )
			{
				m_selected_prefab = entity;
			}
			if ( ImGui::Selectable ( "Remove" ) )
			{
				m_selected_remove = entity;
				m_selected_child_index = childIndex;
			}
			ImGui::EndPopup ();
		}

		// display children entities
		if ( open )
		{
			// display children prefabs
			for ( auto const& child_prefab : data.m_prefab_children )
			{
				ImGui::PushID ( m_uid++ );
				if ( ImGui::TreeNodeEx ( ( std::string ( "[p] " ) + std::get<1> ( std::get<1> ( prefabs[ child_prefab ] )[ 0 ] ).m_name ).c_str () , ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet ) )
				{
					ImGui::TreePop ();
				}
				ImGui::PopID ();
			}

			// Creating parent as child of entity
			for ( auto i = 0; i < data.m_children.size (); ++i )
			{
				RecursivelyDisplaySceneHierarchy ( engineResources , data.m_children[ i ] , entityData , prefabs , i );
			}
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::DisplayPrefab ( EnttXSol::Prefab const& prefab )
	{
		ImGui::PushID ( m_uid++ );
		if ( ImGui::TreeNodeEx ( ( std::string ( "[p] " ) + std::get<1> ( std::get<1> ( prefab )[ 0 ] ).m_name ).c_str () , ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet ) )
		{
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}
}