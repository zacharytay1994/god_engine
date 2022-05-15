#include "../pch.h"
#include "Editor.h"

#include <godUtility/FileIO.h>
#include <godUtility/Internal/RapidJSONWrapper.h>

#include <assert.h>

namespace god
{
	void EditorStyle::UpdateStyle ()
	{
		ImGuiStyle& style = ImGui::GetStyle ();

		style.Colors[ ImGuiCol_Button ] = m_color_button;
		style.Colors[ ImGuiCol_MenuBarBg ] = m_color_bg_menubar;
		style.Colors[ ImGuiCol_ButtonHovered ] = m_color_button_hover;
		style.Colors[ ImGuiCol_WindowBg ] = m_color_bg_window;
		style.Colors[ ImGuiCol_Border ] = m_color_border;
		style.Colors[ ImGuiCol_Text ] = m_color_text;
		style.Colors[ ImGuiCol_TitleBg ] = m_color_bg_title;
		style.Colors[ ImGuiCol_TitleBgActive ] = m_color_bg_titleactive;
		style.Colors[ ImGuiCol_PopupBg ] = m_color_bg_popup;
		style.Colors[ ImGuiCol_FrameBg ] = m_color_bg_frame;
		style.Colors[ ImGuiCol_Separator ] = m_color_seperator;
		style.Colors[ ImGuiCol_TitleBgCollapsed ] = m_color_bg_titlecollapsed;

		style.WindowRounding = m_window_rounding;
	}

	EditorStyle::EditorStyle ()
	{
	}

	void EditorStyle::SetImGuiFont ( char const* font )
	{
		m_font = ImGui::GetIO ().Fonts->AddFontFromFileTTF (
			font , m_font_size );
	}

	void EditorStyle::PushFont ()
	{
		ImGui::PushFont ( m_font );
	}

	void EditorStyle::PopFont ()
	{
		ImGui::PopFont ();
	}

	void JSONifyVec4Wrap ( rapidjson::Document& document , std::string const& theme , char const* name , ImVec4 const& value )
	{
		document[ theme.c_str () ][ name ][ 0 ] = value.x;
		document[ theme.c_str () ][ name ][ 1 ] = value.y;
		document[ theme.c_str () ][ name ][ 2 ] = value.z;
		document[ theme.c_str () ][ name ][ 3 ] = value.w;
	}

	void EditorStyle::JSONify ()
	{
		// should assert if its an editor style jsons
		// assert(es)

		rapidjson::Document document;
		document.SetObject ();

		// open save file
		god::ReadJSON ( document , m_save_path );

		// set current theme
		assert ( document[ "Current Theme" ].IsString () );
		document[ "Current Theme" ].SetString ( m_current_theme.c_str () , document.GetAllocator () );

		// set theme values
		JSONifyVec4Wrap ( document , m_current_theme , "Button" , m_color_button );
		JSONifyVec4Wrap ( document , m_current_theme , "Button Hover" , m_color_button_hover );
		JSONifyVec4Wrap ( document , m_current_theme , "Menu Bar Bg" , m_color_bg_menubar );
		JSONifyVec4Wrap ( document , m_current_theme , "Window Bg" , m_color_bg_window );
		JSONifyVec4Wrap ( document , m_current_theme , "Title Bg" , m_color_bg_title );
		JSONifyVec4Wrap ( document , m_current_theme , "Title Active Bg" , m_color_bg_titleactive );
		JSONifyVec4Wrap ( document , m_current_theme , "Title Collapsed Bg" , m_color_bg_titlecollapsed );
		JSONifyVec4Wrap ( document , m_current_theme , "Popup Bg" , m_color_bg_popup );
		JSONifyVec4Wrap ( document , m_current_theme , "Frame Bg" , m_color_bg_frame );
		JSONifyVec4Wrap ( document , m_current_theme , "Border" , m_color_border );
		JSONifyVec4Wrap ( document , m_current_theme , "Text" , m_color_text );
		JSONifyVec4Wrap ( document , m_current_theme , "Seperator" , m_color_seperator );

		god::WriteJSON ( document , m_save_path );
	}

	void DeJSONifyVec4Wrap ( rapidjson::Value& document , char const* name , ImVec4& value )
	{
		assert ( document[ name ].IsArray () );
		value = {
			document[ name ][ 0 ].GetFloat (),
			document[ name ][ 1 ].GetFloat (),
			document[ name ][ 2 ].GetFloat (),
			document[ name ][ 3 ].GetFloat ()
		};
	}

	void EditorStyle::DeJSONify ( bool current )
	{
		rapidjson::Document document;
		document.SetObject ();

		god::ReadJSON ( document , m_save_path.c_str () );

		assert ( document[ "Current Theme" ].IsString () );

		if ( !current )
		{
			m_current_theme = document[ "Current Theme" ].GetString ();
		}

		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Button" , m_color_button );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Button Hover" , m_color_button_hover );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Menu Bar Bg" , m_color_bg_menubar );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Window Bg" , m_color_bg_window );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Title Bg" , m_color_bg_title );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Title Active Bg" , m_color_bg_titleactive );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Title Collapsed Bg" , m_color_bg_titlecollapsed );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Popup Bg" , m_color_bg_popup );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Frame Bg" , m_color_bg_frame );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Border" , m_color_border );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Text" , m_color_text );
		DeJSONifyVec4Wrap ( document[ m_current_theme.c_str () ] , "Seperator" , m_color_seperator );
	}
}