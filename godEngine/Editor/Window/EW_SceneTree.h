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
		Entity m_selected_entity { EnttXSol::NullEntity };
		std::string m_create_entity_name { "no_name" };
		Entity m_selected_parent { EnttXSol::NullEntity };
		Entity m_selected_parent_temp { m_selected_parent };
		EnttXSol& m_enttxsol;

		void RecursivelyDisplaySceneHierarchy ( Entity entity , std::vector<EnttXSol::EntityData> const& entityData );
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
		if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			ImGui::OpenPopup ( "Create Entity" );
		}
		if ( m_selected_parent != EnttXSol::NullEntity )
		{
			ImGui::OpenPopup ( "Create Child Entity" );
			m_selected_parent_temp = m_selected_parent;
			m_selected_parent = EnttXSol::NullEntity;
		}
		for ( auto i = 0; i < entity_data.size (); ++i )
		{
			if ( entity_data[ i ].m_parent == EnttXSol::NullEntity )
			{
				RecursivelyDisplaySceneHierarchy ( i , entity_data );
			}
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline Entity EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		return m_selected_entity;
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_SceneTree<EDITOR_RESOURCES>::RecursivelyDisplaySceneHierarchy ( Entity entity , std::vector<EnttXSol::EntityData> const& entityData )
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
			ImGui::EndPopup ();
		}
		if ( open )
		{
			// Creating parent as child of entity
			for ( auto i = 0; i < data.m_children.size (); ++i )
			{
				RecursivelyDisplaySceneHierarchy ( data.m_children[ i ] , entityData );
			}
			ImGui::TreePop ();
		}
		ImGui::PopID ();
	}
}