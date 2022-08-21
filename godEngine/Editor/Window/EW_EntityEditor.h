#pragma once
#include "../Editor.h"
#include "../../EnttXSol/EnttXSol.h"

#include "../../imgui/imgui_stdlib.h" 

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_EntityEditor : EditorWindow<EDITOR_RESOURCES>
	{
		EW_EntityEditor ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

	private:
		//Entity m_selected_entity { EnttXSol::NullEntity };
		//std::string m_create_entity_name { "no_name" };
		std::string m_selected_script { "" };
		std::string m_selected_system { "" };

		EnttXSol& m_enttxsol;
	};
}

#include "../../EnttXSol/EngineComponents/EC_All.h"
#include "EW_SceneTree.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_EntityEditor<EDITOR_RESOURCES>::EW_EntityEditor ( EnttXSol& enttxsol )
		:
		m_enttxsol { enttxsol }
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EntityEditor<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		( dt );
		auto scene_tree = this->Get<EW_SceneTree> ();

		ImGui::Begin ("Entity Editor");

		// imgui popup to handle on attach script button click
		if ( ImGui::BeginPopup ( "Attach Script" ) )
		{
			if ( ImGui::BeginListBox ( "##Scripts" ) )
			{
				auto i { 0 };
				for ( auto const& script : m_enttxsol.GetScripts () )
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
						m_enttxsol.AttachScript< EngineComponents > ( scene_tree->GetSelectedEntity() , m_selected_script );
					}
					m_selected_script.clear ();
					ImGui::CloseCurrentPopup ();
				}
				ImGui::EndListBox ();
			}
			ImGui::EndPopup ();
		}

		if ( ImGui::BeginPopup ( "Attach Script System" ) )
		{
			if ( ImGui::BeginListBox ( "##Systems" ) )
			{
				auto i { 0 };
				for ( auto const& script : m_enttxsol.GetScripts () )
				{
					for ( auto const& system : script.second.m_systems )
					{
						ImGui::PushID ( i );
						if ( ImGui::Selectable ( system.first.c_str () , m_selected_system == system.first ) )
						{
							m_selected_system = system.first;
						}
						ImGui::PopID ();
						++i;
					}
				}
				if ( ImGui::Button ( "Attach System" ) )
				{
					if ( !m_selected_system.empty () )
					{
						m_enttxsol.AttachScriptSystem< EngineComponents > ( scene_tree->GetSelectedEntity () , m_selected_system );
					}
					m_selected_system.clear ();
					ImGui::CloseCurrentPopup ();
				}
				ImGui::EndListBox ();
			}
			ImGui::EndPopup ();
		}

		if ( ImGui::BeginPopup ( "Attach Engine Component" ) )
		{
			for ( auto i = 0; i < EngineComponents::m_component_names.size (); ++i )
			{
				if ( ImGui::Selectable ( EngineComponents::m_component_names.at ( i ).c_str () ) )
				{
					m_enttxsol.AttachEngineComponent ( scene_tree->GetSelectedEntity () , i );
				}
			}
			ImGui::EndPopup ();
		}

		if ( scene_tree->GetSelectedEntity () != EnttXSol::Entities::Null )
		{
			// display all engine components
			int uid { 0 };
			m_enttxsol.SerializeEngineComponents< EngineComponents , EDITOR_RESOURCES > ( scene_tree->GetSelectedEntity () , uid , editorResources );

			// display all scripts components
			m_enttxsol.SerializeScriptComponents ( scene_tree->GetSelectedEntity () , uid ,
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
			// attach script system
			if ( ImGui::Button ( "Attach Script System" , { ImGui::GetWindowWidth (), 0.0f } ) )
			{
				ImGui::OpenPopup ( "Attach Script System" );
			}

			// attach engine components
			if ( ImGui::Button ( "Attach Engine Component" , { ImGui::GetWindowWidth (), 0.0f } ) )
			{
				ImGui::OpenPopup ( "Attach Engine Component" );
			}
		}

		ImGui::End ();

		//ImGui::Begin ( "SceneManager" );

		//if ( ImGui::BeginTabBar ( "SceneTree Toggle" ) )
		//{
		//	if ( ImGui::BeginTabItem ( "Tree" ) )
		//	{
		//		if ( ImGui::BeginPopup ( "Create Entity" ) )
		//		{
		//			ImGui::InputText ( "##EntityName" , &m_create_entity_name );
		//			if ( ImGui::Button ( "Create With Name" ) )
		//			{
		//				m_enttxsol.CreateEntity ( m_create_entity_name );
		//				m_create_entity_name = { "no_name" };
		//			}
		//			ImGui::EndPopup ();
		//		}
		//		if ( ImGui::Button ( "Add Entity" , { ImGui::GetWindowWidth () , 0.0f } ) )
		//		{
		//			ImGui::OpenPopup ( "Create Entity" );
		//		}
		//		for ( auto i = 0; i < entity_names.size (); ++i )
		//		{
		//			ImGui::PushID ( i );
		//			if ( ImGui::Selectable ( entity_names[ i ].c_str () ) )
		//			{
		//				m_selected_entity = i;
		//			}
		//			ImGui::PopID ();
		//		}
		//		ImGui::EndTabItem ();
		//	}
		//	if ( ImGui::BeginTabItem ( "Entity" ) )
		//	{
		//		// imgui popup to handle on attach script button click
		//		if ( ImGui::BeginPopup ( "Attach Script" ) )
		//		{
		//			if ( ImGui::BeginListBox ( "##Scripts" ) )
		//			{
		//				auto i { 0 };
		//				for ( auto const& script : m_enttxsol.GetScripts () )
		//				{
		//					ImGui::PushID ( i );
		//					if ( ImGui::Selectable ( script.first.c_str () , m_selected_script == script.first ) )
		//					{
		//						m_selected_script = script.first;
		//					}
		//					ImGui::PopID ();
		//					++i;
		//				}
		//				if ( ImGui::Button ( "Attach Script" ) )
		//				{
		//					if ( !m_selected_script.empty () )
		//					{
		//						m_enttxsol.AttachScript< EngineComponents > ( m_selected_entity , m_selected_script );
		//					}
		//					m_selected_script.clear ();
		//					ImGui::CloseCurrentPopup ();
		//				}
		//				ImGui::EndListBox ();
		//			}
		//			ImGui::EndPopup ();
		//		}

		//		if ( ImGui::BeginPopup ( "Attach Script System" ) )
		//		{
		//			if ( ImGui::BeginListBox ( "##Systems" ) )
		//			{
		//				auto i { 0 };
		//				for ( auto const& script : m_enttxsol.GetScripts () )
		//				{
		//					for ( auto const& system : script.second.m_systems )
		//					{
		//						ImGui::PushID ( i );
		//						if ( ImGui::Selectable ( system.first.c_str () , m_selected_system == system.first ) )
		//						{
		//							m_selected_system = system.first;
		//						}
		//						ImGui::PopID ();
		//						++i;
		//					}
		//				}
		//				if ( ImGui::Button ( "Attach System" ) )
		//				{
		//					if ( !m_selected_system.empty () )
		//					{
		//						m_enttxsol.AttachScriptSystem< EngineComponents > ( m_selected_entity , m_selected_system );
		//					}
		//					m_selected_system.clear ();
		//					ImGui::CloseCurrentPopup ();
		//				}
		//				ImGui::EndListBox ();
		//			}
		//			ImGui::EndPopup ();
		//		}
		//		
		//		if ( m_selected_entity != EnttXSol::NullEntity )
		//		{
		//			// display all engine components
		//			int uid { 0 };
		//			m_enttxsol.SerializeEngineComponents< EngineComponents , EDITOR_RESOURCES > ( m_selected_entity , uid , editorResources );

		//			// display all scripts components
		//			m_enttxsol.SerializeScriptComponents ( m_selected_entity , uid ,
		//				[]( std::string const& name )
		//				{
		//					ImGui::Separator ();
		//					ImGui::Text ( name.c_str () );
		//					ImGui::Separator ();
		//				} ,
		//				[]( bool& val , int i , std::string const& name )
		//				{
		//					ImGui::PushID ( i );
		//					ImGui::Checkbox ( name.c_str () , &val );
		//					ImGui::PopID ();
		//				} ,
		//					[]( int& val , int i , std::string const& name )
		//				{
		//					ImGui::PushID ( i );
		//					ImGui::InputInt ( name.c_str () , &val );
		//					ImGui::PopID ();
		//				} ,
		//					[]( float& val , int i , std::string const& name )
		//				{
		//					ImGui::PushID ( i );
		//					ImGui::InputFloat ( name.c_str () , &val );
		//					ImGui::PopID ();
		//				} ,
		//					[]( std::string& val , int i , std::string const& name )
		//				{
		//					ImGui::PushID ( i );
		//					ImGui::InputText ( name.c_str () , &val );
		//					ImGui::PopID ();
		//				}
		//				);

		//			// attach script button
		//			if ( ImGui::Button ( "Attach Script" , { ImGui::GetWindowWidth () , 0.0f } ) )
		//			{
		//				ImGui::OpenPopup ( "Attach Script" );
		//			}
		//			// attach script system
		//			if ( ImGui::Button ( "Attach Script System" , { ImGui::GetWindowWidth (), 0.0f } ) )
		//			{
		//				ImGui::OpenPopup ( "Attach Script System" );
		//			}
		//		}
		//		ImGui::EndTabItem ();
		//	}
		//	ImGui::EndTabBar ();
		//}

		//if ( ImGui::Button ( "Close" , { ImGui::GetWindowWidth () , 0.0f } ) )
		//{
		//	// reset input 
		//	this->m_open = false;
		//}

		//ImGui::End ();
	}
}