#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#pragma comment (lib, "glfw3.lib")

#include <tuple>
#include <assert.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <assert.h>

#include "../Window/GLFWWindow.h"

namespace god
{
	struct EditorStyle
	{
		ImVec4 m_color_0 { 0.0f, 0.0f, 0.0f, 1.0f };
		ImVec4 m_color_1 { 0.5f, 0.5f, 0.5f, 1.0f };
		ImVec4 m_color_2 { 0.6f, 0.6f, 0.6f, 1.0f };
		ImVec4 m_color_3 { 0.8f, 0.8f, 0.8f, 1.0f };
		ImVec4 m_color_4 { 0.9f, 0.9f, 0.9f, 1.0f };

		ImVec4 m_color_button { m_color_2 };
		ImVec4 m_color_button_hover { m_color_1 };
		ImVec4 m_color_bg_menubar { m_color_2 };
		ImVec4 m_color_bg_window { m_color_1 };
		ImVec4 m_color_bg_title { m_color_2 };
		ImVec4 m_color_bg_titleactive { m_color_2 };
		ImVec4 m_color_bg_titlecollapsed { m_color_2 };
		ImVec4 m_color_bg_popup { m_color_3 };
		ImVec4 m_color_bg_frame { m_color_4 };
		ImVec4 m_color_border { m_color_1 };
		ImVec4 m_color_text { m_color_0 };
		ImVec4 m_color_seperator { m_color_3 };

		std::string m_font_path { "Assets/EngineAssets/Editor/ImGuiFonts/arial.ttf" }; // default font
		ImFont* m_font { nullptr };
		float		m_font_size { 20.0f };

		std::string m_current_theme { "Custom Theme" };
		std::string m_save_path { "Assets/EngineAssets/Editor/Themes/et_themes.json" };

		float m_window_rounding { 10.0f };

		EditorStyle ();
		void UpdateStyle ();
		void SetImGuiFont ( char const* font );
		void PushFont ();
		void PopFont ();

		void JSONify ();
		void DeJSONify ( bool current = false );
	};

	template <typename ENGINE_RESOURCES>
	struct EditorWindows;

	template <typename ENGINE_RESOURCES>
	struct EditorWindow
	{
		bool m_open { true };

		void BaseUpdate ( float dt , ENGINE_RESOURCES& engineResources );
		virtual void Update ( float dt , ENGINE_RESOURCES& editorResources ) = 0;
		void Open ();
		virtual void OnOpen ();
		void Close ();
		virtual void OnClose ();

		friend struct EditorWindows<ENGINE_RESOURCES>;
	protected:
		std::string m_name { "" };
		EditorWindows<ENGINE_RESOURCES>* m_editor_windows { nullptr };
		template <template <typename T> class WINDOW>
		std::shared_ptr<WINDOW<ENGINE_RESOURCES>> Get ();
	};

	template <typename ENGINE_RESOURCES>
	struct EditorWindows
	{
		EditorStyle m_editor_style;

		EditorWindows ();

		template <template<typename T> class WINDOW , typename...CONSTRUCTOR_ARGS>
		void AddWindow ( bool startOpen , CONSTRUCTOR_ARGS ...args );
		template <template<typename T> class WINDOW>
		std::shared_ptr<WINDOW<ENGINE_RESOURCES>> GetWindow ();
		std::unordered_map<std::string , std::shared_ptr<EditorWindow<ENGINE_RESOURCES>>> const& GetWindows ();

		void Update ( float dt , ENGINE_RESOURCES& editorResources );
	private:
		std::unordered_map<std::string , std::shared_ptr<EditorWindow<ENGINE_RESOURCES>>> m_windows;
	};

	template<typename ENGINE_RESOURCES>
	inline void EditorWindow<ENGINE_RESOURCES>::BaseUpdate ( float dt , ENGINE_RESOURCES& engineResources )
	{
		if ( m_open )
		{
			Update ( dt , engineResources );
		}
	}

	template<typename ENGINE_RESOURCES>
	inline void EditorWindow<ENGINE_RESOURCES>::Open ()
	{
		m_open = true;
		OnOpen ();
	}

	template<typename ENGINE_RESOURCES>
	inline void EditorWindow<ENGINE_RESOURCES>::OnOpen ()
	{
	}

	template<typename ENGINE_RESOURCES>
	inline void EditorWindow<ENGINE_RESOURCES>::Close ()
	{
		m_open = false;
		OnClose ();
	}

	template<typename ENGINE_RESOURCES>
	inline void EditorWindow<ENGINE_RESOURCES>::OnClose ()
	{
	}

	template<typename ENGINE_RESOURCES>
	template <template <typename T> class WINDOW>
	inline std::shared_ptr<WINDOW<ENGINE_RESOURCES>> EditorWindow<ENGINE_RESOURCES>::Get ()
	{
		assert ( m_editor_windows->GetWindows ().find ( typeid( WINDOW ).name () ) != m_editor_windows->GetWindows ().end () );
		return std::dynamic_pointer_cast< WINDOW<ENGINE_RESOURCES> >( m_editor_windows->GetWindows ().at ( typeid( WINDOW ).name () ) );
	}

	template<typename ENGINE_RESOURCES>
	inline EditorWindows<ENGINE_RESOURCES>::EditorWindows ()
	{
		m_editor_style.SetImGuiFont ( m_editor_style.m_font_path.c_str () );
	}

	template<typename ENGINE_RESOURCES>
	template<template <typename T> class WINDOW , typename...CONSTRUCTOR_ARGS>
	inline void EditorWindows<ENGINE_RESOURCES>::AddWindow ( bool startOpen , CONSTRUCTOR_ARGS ...args )
	{
		std::string name { typeid( WINDOW ).name () };
		m_windows.insert ( { name , std::make_shared<WINDOW<ENGINE_RESOURCES>> ( args ... ) } );
		m_windows.at ( name )->m_editor_windows = this;
		m_windows.at ( name )->m_name = name;
		m_windows.at ( name )->m_open = startOpen;
	}

	template<typename ENGINE_RESOURCES>
	template<template <typename T> class WINDOW>
	inline std::shared_ptr<WINDOW<ENGINE_RESOURCES>> EditorWindows<ENGINE_RESOURCES>::GetWindow ()
	{
		assert ( m_windows.find ( typeid( WINDOW ).name () ) != m_windows.end () );
		return std::dynamic_pointer_cast< WINDOW<ENGINE_RESOURCES> >( m_windows.at ( typeid( WINDOW ).name () ) );
	}

	template<typename ENGINE_RESOURCES>
	inline void EditorWindows<ENGINE_RESOURCES>::Update ( float dt , ENGINE_RESOURCES& editorResources )
	{
		ImGui::DockSpaceOverViewport ( ImGui::GetMainViewport () );
		int i = 0;
		m_editor_style.PushFont ();
		for ( auto& window : m_windows )
		{
			ImGui::PushID ( i++ );
			window.second->BaseUpdate ( dt , editorResources );
			ImGui::PopID ();
		}
		m_editor_style.PopFont ();
	}

	template<typename ENGINE_RESOURCES>
	inline std::unordered_map<std::string , std::shared_ptr<EditorWindow<ENGINE_RESOURCES>>> const& EditorWindows<ENGINE_RESOURCES>::GetWindows ()
	{
		return m_windows;
	}
}