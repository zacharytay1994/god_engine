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

struct GLFWwindow;
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
}