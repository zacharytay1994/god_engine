#pragma once
#include "../Editor.h"

#include "../../imgui/imgui_stdlib.h" 

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_SceneManager : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

	private:
	};
}

#include "../../EnttXSol/EnttXSol.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void EW_SceneManager<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		( dt );

		auto& enttxsol = editorResources.Get<EnttXSol> ().get ();
		auto& entity_names = enttxsol.GetEntityNames ();

		ImGui::Begin ( "SceneManager" );

		if ( ImGui::BeginTabBar ( "SceneTree Toggle" ) )
		{
			if ( ImGui::BeginTabItem ( "Tree" ) )
			{
				if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
				{
					enttxsol.AttachScript ( enttxsol.CreateEntity ( "Test" ) , "test" );
				}
				for ( auto i = 0; i < entity_names.size (); ++i )
				{
					ImGui::Text ( entity_names[ i ].c_str () );
				}
				ImGui::EndTabItem ();
			}
			if ( ImGui::BeginTabItem ( "Entity" ) )
			{
				ImGui::EndTabItem ();
			}
			ImGui::EndTabBar ();
		}

		if ( ImGui::Button ( "Close" , { ImGui::GetWindowWidth () , 0.0f } ) )
		{
			// reset input 
			this->m_open = false;
		}

		ImGui::End ();
	}
}