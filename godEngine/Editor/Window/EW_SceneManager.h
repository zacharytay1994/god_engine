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
		Entity m_selected_entity { EnttXSol::NullEntity };
		std::string m_create_entity_name { "no_name" };
		std::string m_selected_script { "" };
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
		auto& engine_components = editorResources.Get<3> ().get ();
		auto& entity_names = enttxsol.GetEntityNames ();

		ImGui::Begin ( "SceneManager" );

		if ( ImGui::BeginTabBar ( "SceneTree Toggle" ) )
		{
			if ( ImGui::BeginTabItem ( "Tree" ) )
			{
				if ( ImGui::BeginPopup ( "Create Entity" ) )
				{
					ImGui::InputText ( "##EntityName" , &m_create_entity_name );
					if ( ImGui::Button ( "Create With Name" ) )
					{
						//enttxsol.AttachComponent<Transform> ( enttxsol.CreateEntity ( m_create_entity_name ) );
						enttxsol.CreateEntity ( m_create_entity_name );
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
				ImGui::EndTabItem ();
			}
			if ( ImGui::BeginTabItem ( "Entity" ) )
			{
				// imgui popup to handle on attach script button click
				if ( ImGui::BeginPopup ( "Attach Script" ) )
				{
					if ( ImGui::BeginListBox ( "##Scripts" ) )
					{
						auto i { 0 };
						for ( auto const& script : enttxsol.GetScripts () )
						{
							ImGui::PushID ( i );
							if ( ImGui::Selectable ( script.first.c_str () , m_selected_script == script.first ) )
							{
								m_selected_script = script.first;
							}
							ImGui::PopID ();
							++i;
						}
						if ( ImGui::Button ( "Attach Script" ) )
						{
							if ( !m_selected_script.empty () )
							{
								enttxsol.AttachScript ( m_selected_entity , m_selected_script , engine_components );
							}
							m_selected_script.clear ();
							ImGui::CloseCurrentPopup ();
						}
						ImGui::EndListBox ();
					}
					ImGui::EndPopup ();
				}

				if ( m_selected_entity != EnttXSol::NullEntity )
				{
					// display all engine components
					int uid { 0 };
					enttxsol.SerializeEngineComponents ( m_selected_entity , uid , engine_components );

					// display all scripts components
					enttxsol.SerializeScriptComponents ( m_selected_entity , uid ,
						[]( std::string const& name )
						{
							ImGui::Separator ();
							ImGui::Text ( name.c_str () );
							ImGui::Separator ();
						} ,
						[]( bool& val , int i , std::string const& name )
						{
							ImGui::PushID ( i );
							ImGui::Checkbox ( name.c_str () , &val );
							ImGui::PopID ();
						} ,
							[]( int& val , int i , std::string const& name )
						{
							ImGui::PushID ( i );
							ImGui::InputInt ( name.c_str () , &val );
							ImGui::PopID ();
						} ,
							[]( float& val , int i , std::string const& name )
						{
							ImGui::PushID ( i );
							ImGui::InputFloat ( name.c_str () , &val );
							ImGui::PopID ();
						} ,
							[]( std::string& val , int i , std::string const& name )
						{
							ImGui::PushID ( i );
							ImGui::InputText ( name.c_str () , &val );
							ImGui::PopID ();
						}
						);

					// attach script button
					if ( ImGui::Button ( "Attach Script" , { ImGui::GetWindowWidth () , 0.0f } ) )
					{
						ImGui::OpenPopup ( "Attach Script" );
					}
				}
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