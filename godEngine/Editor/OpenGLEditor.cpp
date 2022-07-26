#include "../pch.h"
#include "OpenGLEditor.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

namespace god
{
	ImGuiOpenGLEditor::ImGuiOpenGLEditor ( GLFWWindow& glfwWindow )
	{
		char const* glsl_version = "#version 430";

		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();
		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		// multi viewport buggy
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark ();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle ();
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			style.WindowRounding = 0.0f;
			style.Colors[ ImGuiCol_WindowBg ].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL ( glfwWindow.GetGLFWWindow () , true );
		ImGui_ImplOpenGL3_Init ( glsl_version );
	}

	ImGuiOpenGLEditor::~ImGuiOpenGLEditor ()
	{
		ImGui_ImplOpenGL3_Shutdown ();
		ImGui_ImplGlfw_Shutdown ();
		ImGui::DestroyContext ();
	}

	void ImGuiOpenGLEditor::BeginFrame ()
	{
		ImGui_ImplOpenGL3_NewFrame ();
		ImGui_ImplGlfw_NewFrame ();
		ImGui::NewFrame ();
	}

	void ImGuiOpenGLEditor::Render ()
	{
		ImGui::Render ();
	}

	void ImGuiOpenGLEditor::EndFrame ()
	{
		ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData () );

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext ();
			ImGui::UpdatePlatformWindows ();
			ImGui::RenderPlatformWindowsDefault ();
			glfwMakeContextCurrent ( backup_current_context );
		}
	}
}