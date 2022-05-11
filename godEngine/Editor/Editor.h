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

	template <typename EDITOR_RESOURCES>
	struct EditorWindows;

	template <typename EDITOR_RESOURCES>
	struct EditorWindow
	{
		virtual void Update ( float dt , EDITOR_RESOURCES& editorResources ) = 0;

		friend struct EditorWindows<EDITOR_RESOURCES>;
	protected:
		std::string m_name { "" };
		EditorWindows<EDITOR_RESOURCES>* m_editor_windows { nullptr };
		template <template <typename T> class WINDOW>
		std::shared_ptr<WINDOW<EDITOR_RESOURCES>> Get ();

		//std::shared_ptr<EditorWindow<EDITOR_RESOURCES>> Get ( std::string const& name );
	};

	template <typename EDITOR_RESOURCES>
	struct EditorWindows
	{
		template <template<typename T> class WINDOW>
		void AddWindow ();

		void Update ( float dt , EDITOR_RESOURCES& editorResources );

		std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> const& GetWindows ();
	private:
		std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> m_windows;
	};

	template<typename EDITOR_RESOURCES>
	template <template <typename T> class WINDOW>
	inline std::shared_ptr<WINDOW<EDITOR_RESOURCES>> EditorWindow<EDITOR_RESOURCES>::Get ()
	{
		assert ( m_editor_windows->GetWindows ().find ( typeid( WINDOW ).name () ) != m_editor_windows->GetWindows ().end () );
		return std::dynamic_pointer_cast< WINDOW<EDITOR_RESOURCES> >( m_editor_windows->GetWindows ().at ( typeid( WINDOW ).name () ) );
	}
	/*template<typename EDITOR_RESOURCES>
	inline std::shared_ptr<EditorWindow<EDITOR_RESOURCES>> EditorWindow<EDITOR_RESOURCES>::Get ( std::string const& name )
	{
		assert ( m_editor_windows->GetWindows ().find ( name ) != m_editor_windows->GetWindows ().end ()
			&& ( name + " : Trying to get EditorWindow that does not exist" ).c_str () );
		return m_editor_windows->GetWindows ().at ( name );
	}*/

	template<typename EDITOR_RESOURCES>
	template<template <typename T> class WINDOW>
	inline void EditorWindows<EDITOR_RESOURCES>::AddWindow ( )
	{
		std::string name { typeid( WINDOW ).name () };
		m_windows.insert ( { name , std::make_shared<WINDOW<EDITOR_RESOURCES>> () } );
		m_windows.at ( name )->m_editor_windows = this;
		m_windows.at ( name )->m_name = name;
	}

	template<typename EDITOR_RESOURCES>
	inline void EditorWindows<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		for ( auto& window : m_windows )
		{
			window.second->Update ( dt , editorResources );
		}
	}

	template<typename EDITOR_RESOURCES>
	inline std::unordered_map<std::string , std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> const& EditorWindows<EDITOR_RESOURCES>::GetWindows ()
	{
		return m_windows;
	}
}