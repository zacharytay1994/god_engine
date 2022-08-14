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
		std::string m_create_entity_name { "no_name" };
		Entity m_selected_entity { EnttXSol::NullEntity };
		Entity m_selected_parent { EnttXSol::NullEntity };
		Entity m_selected_parent_temp { m_selected_parent };
		Entity m_selected_remove { EnttXSol::NullEntity };
		Entity m_selected_child_index { EnttXSol::NullEntity }; // index representing position in the children container of its parent

		EnttXSol& m_enttxsol;

		void RecursivelyDisplaySceneHierarchy ( Entity entity , std::vector<EnttXSol::EntityData> const& entityData , uint32_t childIndex = EnttXSol::NullEntity );
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
		( editorResources );
		auto& entities = m_enttxsol.GetEntities ();
		auto& entity_data = m_enttxsol.GetEntityData ();

		ImGui::Begin ( "Scene Tree" );
		// Creating entity without parent
		if ( ImGui::BeginPopup ( "Create Entity" ) )
		{
			ImGui::InputText ( "##EntityName" , &m_create_entity_name );
			if ( ImGui::Button ( "Create With Name" ) )
			{
				m_enttxsol.CreateEntity ( m_create_entity_name );
				m_create_entity_name = { "no_name" };
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		if ( ImGui::BeginPopup ( "Create Child Entity" ) )
		{
			ImGui::InputText ( "##EntityName" , &m_create_entity_name );
			if ( ImGui::Button ( "Create With Name" ) )
			{
				m_enttxsol.CreateEntity ( m_create_entity_name , m_selected_parent_temp );
				m_create_entity_name = { "no_name" };
				m_selected_parent_temp = EnttXSol::NullEntity;
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
		// removing entity
		if ( m_selected_remove != EnttXSol::NullEntity )
		{
			m_enttxsol.RemoveEntity ( m_selected_remove , m_selected_child_index );
			m_selected_entity = EnttXSol::NullEntity;
			m_selected_remove = EnttXSol::NullEntity;
			m_selected_child_index = EnttXSol::NullEntity;
		}
		// render scene hierarchy
		for ( auto i = 0; i < entities.size (); ++i )
		{
			if ( entities[i].has_value() && entity_data[ i ].m_parent == EnttXSol::NullEntity )
			{
				RecursivelyDisplaySceneHierarchy ( i , entity_data );
			}
		}

		// test serialize
		if ( ImGui::Button ( "Serialize" ) )
		{
			m_enttxsol.SerializeState ();
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline Entity EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( Entity entity , std::vector<EnttXSol::EntityData> const& entityData , uint32_t childIndex )
	{
		ImGui::PushID ( entity );
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
			if ( ImGui::Selectable ( "Remove" ) )
			{
				m_selected_remove = entity;
				m_selected_child_index = childIndex;
			}
			ImGui::EndPopup ();
		}
		if ( open )
		{
			// Creating parent as child of entity
			for ( auto i = 0; i < data.m_children.size (); ++i )
			{
				RecursivelyDisplaySceneHierarchy ( data.m_children[ i ] , entityData , i );
			}
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}
}