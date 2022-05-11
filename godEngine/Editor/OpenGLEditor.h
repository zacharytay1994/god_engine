#pragma once

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

		void BeginFrame ();
		void Render ();
		void EndFrame ();
	};
}