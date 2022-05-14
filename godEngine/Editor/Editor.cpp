#include "../pch.h"
#include "Editor.h"

#include <godUtility/FileIO.h>

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

	void JSONifyVec4Wrap ( rapidjson::Document& document , char const* name , ImVec4 const& value )
	{
		RapidJSON::JSONifyValues ( document , name , value.x , value.y , value.z , value.w );
	}

	void EditorStyle::JSONify ( std::string const& json )
	{
		// should assert if its an editor style json
		// assert(es)

		rapidjson::Document document;
		document.SetObject ();

		JSONifyVec4Wrap ( document , "Button"			, m_color_button			);
		JSONifyVec4Wrap ( document , "Button Hover"		, m_color_button_hover		);
		JSONifyVec4Wrap ( document , "Menu Bar Bg"		, m_color_bg_menubar		);
		JSONifyVec4Wrap ( document , "Window Bg"		, m_color_bg_window			);
		JSONifyVec4Wrap ( document , "Title Bg"			, m_color_bg_title			);
		JSONifyVec4Wrap ( document , "Title Active Bg"	, m_color_bg_titleactive	);
		JSONifyVec4Wrap ( document , "Popup Bg"			, m_color_bg_popup			);
		JSONifyVec4Wrap ( document , "Frame Bg"			, m_color_bg_frame			);
		JSONifyVec4Wrap ( document , "Border"			, m_color_border			);
		JSONifyVec4Wrap ( document , "Text"				, m_color_text				);
		JSONifyVec4Wrap ( document , "Seperator"		, m_color_seperator			);

		god::WriteJSON ( document , json.c_str () );
	}

	void DeJSONifyVec4Wrap ( rapidjson::Document& document , char const* name , ImVec4& value )
	{
		assert ( document[ name ].IsArray () );
		value = {
			document[ name ][ 0 ].GetFloat (),
			document[ name ][ 1 ].GetFloat (),
			document[ name ][ 2 ].GetFloat (),
			document[ name ][ 3 ].GetFloat ()
		};
	}

	void EditorStyle::DeJSONify ( std::string const& json )
	{
		rapidjson::Document document;
		document.SetObject ();

		god::ReadJSON ( document , json.c_str () );

		DeJSONifyVec4Wrap ( document , "Button"			, m_color_button			);
		DeJSONifyVec4Wrap ( document , "Button Hover"	, m_color_button_hover		);
		DeJSONifyVec4Wrap ( document , "Menu Bar Bg"	, m_color_bg_menubar		);
		DeJSONifyVec4Wrap ( document , "Window Bg"		, m_color_bg_window			);
		DeJSONifyVec4Wrap ( document , "Title Bg"		, m_color_bg_title			);
		DeJSONifyVec4Wrap ( document , "Title Active Bg", m_color_bg_titleactive	);
		DeJSONifyVec4Wrap ( document , "Popup Bg"		, m_color_bg_popup			);
		DeJSONifyVec4Wrap ( document , "Frame Bg"		, m_color_bg_frame			);
		DeJSONifyVec4Wrap ( document , "Border"			, m_color_border			);
		DeJSONifyVec4Wrap ( document , "Text"			, m_color_text				);
		DeJSONifyVec4Wrap ( document , "Seperator"		, m_color_seperator			);
	}
}