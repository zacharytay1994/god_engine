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
		EnttXSol& m_enttxsol;
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
		auto& entity_names = m_enttxsol.GetEntityNames ();

		ImGui::Begin ( "Scene Tree" );
		if ( ImGui::BeginPopup ( "Create Entity" ) )
		{
			ImGui::InputText ( "##EntityName" , &m_create_entity_name );
			if ( ImGui::Button ( "Create With Name" ) )
			{
				m_enttxsol.CreateEntity ( m_create_entity_name );
				m_create_entity_name = { "no_name" };
			}
			ImGui::EndPopup ();
		}
		if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			ImGui::OpenPopup ( "Create Entity" );
		}
		for ( auto i = 0; i < entity_names.size (); ++i )
		{
			ImGui::PushID ( i );
			if ( ImGui::Selectable ( entity_names[ i ].c_str () ) )
			{
				m_selected_entity = i;
			}
			ImGui::PopID ();
		}
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline Entity EW_SceneTree<EDITOR_RESOURCES>::GetSelectedEntity ()
	{
		return m_selected_entity;
	}
}