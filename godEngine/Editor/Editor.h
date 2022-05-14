#pragma once
#include <godUtility/Structures.h>

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

namespace god
{
	/*!
	 * @brief
	 * : A collection of reference wrappers of type.
	 * @tparam ...TYPES
	 * : The types that the are contained within.
	*/
	template <typename...TYPES>
	using EditorResources = god::ReferenceWrapperCollection<TYPES...>;

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
		ImVec4 m_color_bg_popup { m_color_3 };
		ImVec4 m_color_bg_frame { m_color_4 };
		ImVec4 m_color_border { m_color_1 };
		ImVec4 m_color_text { m_color_0 };
		ImVec4 m_color_seperator { m_color_3 };

		std::string m_font_path { "Assets/EngineAssets/Editor/ImGuiFonts/arial.ttf" }; // default font
		ImFont* m_font { nullptr };
		float		m_font_size { 20.0f };

		EditorStyle ();
		void UpdateStyle ();
		void SetImGuiFont ( char const* font );
		void PushFont ();
		void PopFont ();

		void JSONify ( std::string const& json );
		void DeJSONify ( std::string const& json );
	};

	template <typename EDITOR_RESOURCES>
	struct EditorWindows;

	template <typename EDITOR_RESOURCES>
	struct EditorWindow
	{
		bool m_open { true };

		void BaseUpdate ( float dt , EDITOR_RESOURCES& editorResources );
		virtual void Update ( float dt , EDITOR_RESOURCES& editorResources ) = 0;

		friend struct EditorWindows<EDITOR_RESOURCES>;
	protected:
		std::string m_name { "" };
		EditorWindows<EDITOR_RESOURCES>* m_editor_windows { nullptr };
		template <template <typename T> class WINDOW>
		std::shared_ptr<WINDOW<EDITOR_RESOURCES>> Get ();
	};

	template <typename EDITOR_RESOURCES>
	struct EditorWindows
	{
		EditorStyle m_editor_style;

		EditorWindows ();

		template <template<typename T> class WINDOW>
		void AddWindow ( bool startOpen = false );
		std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> const& GetWindows ();

		void Update ( float dt , EDITOR_RESOURCES& editorResources );
	private:
		std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> m_windows;
	};

	template<typename EDITOR_RESOURCES>
	inline void EditorWindow<EDITOR_RESOURCES>::BaseUpdate ( float dt , EDITOR_RESOURCES& editorResources )
	{
		if ( m_open )
		{
			Update ( dt , editorResources );
		}
	}

	template<typename EDITOR_RESOURCES>
	template <template <typename T> class WINDOW>
	inline std::shared_ptr<WINDOW<EDITOR_RESOURCES>> EditorWindow<EDITOR_RESOURCES>::Get ()
	{
		assert ( m_editor_windows->GetWindows ().find ( typeid( WINDOW ).name () ) != m_editor_windows->GetWindows ().end () );
		return std::dynamic_pointer_cast< WINDOW<EDITOR_RESOURCES> >( m_editor_windows->GetWindows ().at ( typeid( WINDOW ).name () ) );
	}

	template<typename EDITOR_RESOURCES>
	inline EditorWindows<EDITOR_RESOURCES>::EditorWindows ()
	{
		m_editor_style.SetImGuiFont ( m_editor_style.m_font_path.c_str () );
		//m_editor_style.UpdateStyle ();
		//m_editor_style.JSONify ( "Assets/EngineAssets/Editor/Themes/et_light.json" );
	}

	template<typename EDITOR_RESOURCES>
	template<template <typename T> class WINDOW>
	inline void EditorWindows<EDITOR_RESOURCES>::AddWindow ( bool startOpen )
	{
		std::string name { typeid( WINDOW ).name () };
		m_windows.insert ( { name , std::make_shared<WINDOW<EDITOR_RESOURCES>> () } );
		m_windows.at ( name )->m_editor_windows = this;
		m_windows.at ( name )->m_name = name;
		m_windows.at ( name )->m_open = startOpen;
	}

	template<typename EDITOR_RESOURCES>
	inline void EditorWindows<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
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

	template<typename EDITOR_RESOURCES>
	inline std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> const& EditorWindows<EDITOR_RESOURCES>::GetWindows ()
	{
		return m_windows;
	}
}