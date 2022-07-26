#pragma once
#include "../Window/GLFWWindow.h"

#include <memory>

namespace god
{
	/*!
	 * @brief
	 * : Initializes imgui based on GLFW+OpenGL
	*/
	struct ImGuiOpenGLEditor
	{
		ImGuiOpenGLEditor ( GLFWWindow& glfwWindow );
		~ImGuiOpenGLEditor ();

		void BeginFrame ();
		void Render ();
		void EndFrame ();
	};
}