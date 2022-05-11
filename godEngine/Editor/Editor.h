#pragma once
#include "../Window/GLFWWindow.h"
#include <godUtility/Structures.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#pragma comment (lib, "glfw3.lib")

#include <tuple>
#include <assert.h>
#include <vector>
#include <memory>

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

	template <typename EDITOR_RESOURCES>
	struct EditorWindow
	{
		virtual void Update ( float dt , EDITOR_RESOURCES& editorResources ) = 0;
	};

	template <typename EDITOR_RESOURCES>
	struct EditorWindows
	{
		template <template<typename T> class WINDOW>
		void AddWindow ();

		void Update ( float dt , EDITOR_RESOURCES& editorResources );
	private:
		std::vector <std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> m_windows;
	};

	template<typename EDITOR_RESOURCES>
	template<template <typename T> class WINDOW>
	inline void EditorWindows<EDITOR_RESOURCES>::AddWindow ()
	{
		m_windows.push_back ( std::make_shared<WINDOW<EDITOR_RESOURCES>> () );
	}

	template<typename EDITOR_RESOURCES>
	inline void EditorWindows<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		for ( auto& window : m_windows )
		{
			window->Update ( dt , editorResources );
		}
	}
}