#pragma once
#include "../Editor.h"

#include <glm/glm/glm.hpp>
#include <godUtility/FileIO.h>

#include <sstream>

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_EditorStyles : EditorWindow<EDITOR_RESOURCES>
	{
		EditorStyle m_editor_style;

		EW_EditorStyles ();
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;

	private:
		float m_input_window_width { 80.0f };
		void ColourInput ( char const* name , ImVec4& colour );
	};
}

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_EditorStyles<EDITOR_RESOURCES>::EW_EditorStyles ()
	{
		// set font
		m_editor_style.SetImGuiFont ( m_editor_style.m_font_path.c_str () );
		m_editor_style.DeJSONify ();
		m_editor_style.UpdateStyle ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EditorStyles<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		ImGui::Begin ( "Editor Styles" );

		if ( ImGui::BeginPopup ( "Themes" ) )
		{
			if ( ImGui::Selectable ( "Custom Theme" ) )
			{
				m_editor_style.m_current_theme = "Custom Theme";
				m_editor_style.DeJSONify ( true );
				m_editor_style.UpdateStyle ();
				m_editor_style.JSONify ();
			}
			if ( ImGui::Selectable ( "Light Theme" ) )
			{
				m_editor_style.m_current_theme = "Light Theme";
				m_editor_style.DeJSONify ( true );
				m_editor_style.UpdateStyle ();
				m_editor_style.JSONify ();
			}
			if ( ImGui::Selectable ( "Dark Theme" ) )
			{
				m_editor_style.m_current_theme = "Dark Theme";
				m_editor_style.DeJSONify ( true );
				m_editor_style.UpdateStyle ();
				m_editor_style.JSONify ();
			}
			ImGui::EndPopup ();
		}

		if ( ImGui::Button ( m_editor_style.m_current_theme.c_str () ) )
		{
			ImGui::OpenPopup ( "Themes" );
		}

		ColourInput ( "Button"			, m_editor_style.m_color_button			);
		ColourInput ( "Button Hover"	, m_editor_style.m_color_button_hover	);
		ColourInput ( "Menu Bar"		, m_editor_style.m_color_bg_menubar		);
		ColourInput ( "Window"			, m_editor_style.m_color_bg_window		);
		ColourInput ( "Title"			, m_editor_style.m_color_bg_title		);
		ColourInput ( "Title Active"	, m_editor_style.m_color_bg_titleactive );
		ColourInput ( "Title Collapsed" , m_editor_style.m_color_seperator );
		ColourInput ( "Popup"			, m_editor_style.m_color_bg_popup		);
		ColourInput ( "Frame"			, m_editor_style.m_color_bg_frame		);
		ColourInput ( "Border"			, m_editor_style.m_color_border			);
		ColourInput ( "Text"			, m_editor_style.m_color_text			);
		ColourInput ( "Seperator"		, m_editor_style.m_color_seperator		);

		if ( ImGui::Button ( "Save and Apply" ) )
		{
			m_editor_style.UpdateStyle ();
			m_editor_style.JSONify ();
		}

		if ( ImGui::Button ( "Close" ) )
		{
			this->m_open = false;
		}

		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_EditorStyles<EDITOR_RESOURCES>::ColourInput ( char const* name , ImVec4& colour )
	{
		float input_width = ImGui::GetWindowWidth () / 6.0f;

		ImGui::SetNextItemWidth ( input_width );
		std::string label { "##" };
		label += name;
		ImGui::InputFloat ( ( label + "r" ).c_str () , &colour.x );
		ImGui::SameLine ();
		ImGui::SetNextItemWidth ( input_width );
		ImGui::InputFloat ( ( label + "g" ).c_str () , &colour.y );
		ImGui::SameLine ();
		ImGui::SetNextItemWidth ( input_width );
		ImGui::InputFloat ( ( label + "b" ).c_str () , &colour.z );
		ImGui::SameLine ();
		label = name;
		ImGui::SetNextItemWidth ( input_width );
		ImGui::InputFloat ( ( label + "##a" ).c_str () , &colour.w );
	}
}