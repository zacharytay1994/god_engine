#include "../pch.h"
#include "Editor.h"

namespace god
{
	void EditorStyle::UpdateStyle ()
	{
		ImGuiStyle& style = ImGui::GetStyle ();

		style.Colors[ ImGuiCol_Button ]			= m_color_button			;
		style.Colors[ ImGuiCol_MenuBarBg ]		= m_color_bg_menubar		;
		style.Colors[ ImGuiCol_ButtonHovered ]	= m_color_button_hover		;
		style.Colors[ ImGuiCol_WindowBg ]		= m_color_bg_window			;
		style.Colors[ ImGuiCol_Border ]			= m_color_border			;
		style.Colors[ ImGuiCol_Text ]			= m_color_text				;
		style.Colors[ ImGuiCol_TitleBg ]		= m_color_bg_title			;
		style.Colors[ ImGuiCol_TitleBgActive ]	= m_color_bg_titleactive	;
		style.Colors[ ImGuiCol_PopupBg ]		= m_color_bg_popup			;
		style.Colors[ ImGuiCol_FrameBg ]		= m_color_bg_frame			;
		style.Colors[ ImGuiCol_Separator ]		= m_color_seperator			;
	}

	EditorStyle::EditorStyle ()
	{
		LoadImGuiFont ( m_font_path.c_str () );
		ImGui::PushFont ( m_font );
		ImGui::PopFont ();
	}

	void EditorStyle::LoadImGuiFont ( char const* font )
	{
		m_font = ImGui::GetIO ().Fonts->AddFontFromFileTTF (
			font , 15.0f );
	}
}