#pragma once
#include "../Editor.h"
#include "../../EnttXSol/EnttXSol.h"
#include "../../imgui/imgui_stdlib.h"

#include <godUtility/Internal/FolderHelper.h>
#include <godUtility/FileIO.h>

#include <vector>
#include <string>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_EntityEditor : EditorWindow<EDITOR_RESOURCES>
	{
		EW_EntityEditor ( EnttXSol& enttxsol );
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

		bool m_select_inspector_tab { false };

	private:
		std::string m_selected_script { "" };
		std::string m_selected_system { "" };

		EnttXSol& m_enttxsol;

		void Inspector ( EDITOR_RESOURCES& editorResources );

		std::string m_new_script_name { "new_script" };
		std::string m_script_to_delete { "" };
		std::string m_script_to_delete_temp { "" };
		std::vector<std::string> m_scripts;
		int m_selected_script_id { -1 };
		void Scripts ( EDITOR_RESOURCES& editorResources );
		void LoadScripts ();
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
		LoadScripts ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EntityEditor<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		( dt );

		ImGui::Begin ( "Entity Editor" );

		if ( ImGui::BeginTabBar ( "EntityEditorTab" ) )
		{
			ImGuiTabItemFlags flag { 0 };
			if ( m_select_inspector_tab )
			{
				flag |= ImGuiTabItemFlags_SetSelected;
				m_select_inspector_tab = false;
			}

			if ( ImGui::BeginTabItem ( "Inspector" , 0 , flag ) )
			{
				Inspector ( editorResources );
				ImGui::EndTabItem ();
			}

			if ( ImGui::BeginTabItem ( "Scripts" ) )
			{
				Scripts ( editorResources );
				ImGui::EndTabItem ();
			}

			ImGui::EndTabBar ();
		}

		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EntityEditor<EDITOR_RESOURCES>::Inspector ( EDITOR_RESOURCES& editorResources )
	{
		auto scene_tree = this->Get<EW_SceneTree> ();

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
						m_enttxsol.AttachScript< EngineComponents > ( scene_tree->GetSelectedEntity () , m_selected_script );
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
			// entity id
			ImGui::Text ( "Entity ID [%d]" , scene_tree->GetSelectedEntity () );

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
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EntityEditor<EDITOR_RESOURCES>::Scripts ( EDITOR_RESOURCES& editorResources )
	{
		auto scene_tree = this->Get<EW_SceneTree> ();

		// display all scripts
		if ( ImGui::Button ( "Refresh List" , ImVec2 ( ImGui::GetWindowWidth () , 0 ) ) )
		{
			LoadScripts ();
		}
		ImGui::SetNextItemWidth ( ImGui::GetWindowWidth () );

		if ( ImGui::BeginPopup ( "DeleteScript" ) )
		{
			ImGui::Text ( "This is a DESTRUCTIVE action. Confirm?" );
			if ( ImGui::Button ( "Confirm Delete" ) )
			{
				m_enttxsol.DeleteScript ( m_script_to_delete_temp );
				scene_tree->ResetScene ( editorResources );
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		if ( !m_script_to_delete.empty () )
		{
			m_script_to_delete_temp = m_script_to_delete;
			m_script_to_delete.clear ();
			ImGui::OpenPopup ( "DeleteScript" );
		}

		if ( ImGui::BeginListBox ( "##scripts" , ImVec2 ( ImGui::GetWindowWidth () , 0 ) ) )
		{
			int i { 0 };
			for ( auto const& script : m_scripts )
			{
				auto last_dash = script.find_last_of ( '/' );
				auto last_dot = script.find_last_of ( '.' );
				std::string name = script.substr ( last_dash + 1 , last_dot - ( last_dash + 1 ) );
				if ( ImGui::Selectable ( name.c_str () , m_selected_script_id == i ) )
				{
					m_selected_script_id = i;
				}
				this->ToolTipOnHover ( "Right Click for Options." );
				if ( ImGui::BeginPopupContextItem () )
				{
					m_selected_script_id = i;
					if ( ImGui::Selectable ( "Reload" ) )
					{
						m_enttxsol.ReloadScript ( script );
						scene_tree->ResetScene ( editorResources );
					}
					this->ToolTipOnHover ( "Reloads the Script for Runtime Changes." );
					if ( ImGui::Selectable ( "Delete" ) )
					{
						m_script_to_delete = script;
					}
					this->ToolTipOnHover ( "Deletes the Script from the Engine." );
					ImGui::EndPopup ();
				}
				++i;
			}
			ImGui::EndListBox ();
		}

		if ( ImGui::BeginPopup ( "NewScriptFile" ) )
		{
			ImGui::InputText ( "##newscriptname" , &m_new_script_name );
			if ( ImGui::Button ( "Create With Name" ) )
			{
				m_enttxsol.NewScriptTemplate ( m_new_script_name );
				m_enttxsol.LoadScript ( std::string ( "Assets/GameAssets/Scripts/" ) + m_new_script_name + ".lua" );
				m_new_script_name = { "new_script" };
				ImGui::CloseCurrentPopup ();
			}
			ImGui::EndPopup ();
		}
		if ( ImGui::Button ( "New Script" , ImVec2 ( ImGui::GetWindowWidth () , 0 ) ) )
		{
			ImGui::OpenPopup ( "NewScriptFile" );
		}
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EntityEditor<EDITOR_RESOURCES>::LoadScripts ()
	{
		m_scripts = FolderHelper::GetFiles ( "Assets/GameAssets/Scripts/" );
	}
}