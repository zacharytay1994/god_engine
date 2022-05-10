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

	/*!
	 * @brief 
	 * : Takes in an EditorResources object defined with a collection of references.
	 *		Must contain a GLFWWindow reference in order to intitialize imgui.
	 * @tparam EDITOR_RESOURCES 
	 * : The EditorResources type.
	*/
	template <typename EDITOR_RESOURCES>
	struct ImGuiOpenGLEditor
	{
		ImGuiOpenGLEditor ( EDITOR_RESOURCES editorResources );

		void BeginFrame ();
		void Update ( float dt );
		void Render ();
		void EndFrame ();
	private:
		EDITOR_RESOURCES m_editor_resources;
	};

	template<typename EDITOR_RESOURCES>
	ImGuiOpenGLEditor<EDITOR_RESOURCES>::ImGuiOpenGLEditor ( EDITOR_RESOURCES editorResources )
		:
		m_editor_resources { editorResources }
	{
		assert ( m_editor_resources.Has<int> () && "ImGuiOpenGLEditor requires an EDITOR_RESOURCES object with a GLFWWindow." );
		GLFWwindow* window = m_editor_resources.Get<GLFWWindow> ().get ().GetGLFWWindow ();

		char const* glsl_version = "#version 430";

		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();
		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

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
		ImGui_ImplGlfw_InitForOpenGL ( window , true );
		ImGui_ImplOpenGL3_Init ( glsl_version );
	}

	template<typename EDITOR_RESOURCES>
	void ImGuiOpenGLEditor<EDITOR_RESOURCES>::BeginFrame ()
	{
		ImGui_ImplOpenGL3_NewFrame ();
		ImGui_ImplGlfw_NewFrame ();
		ImGui::NewFrame ();
	}

	template<typename EDITOR_RESOURCES>
	void ImGuiOpenGLEditor<EDITOR_RESOURCES>::Update ( float dt )
	{
		ImGui::Begin ( "Test" );
		ImGui::Text ( "Hello world" );
		ImGui::End ();
	}

	template<typename EDITOR_RESOURCES>
	void ImGuiOpenGLEditor<EDITOR_RESOURCES>::Render ()
	{
		ImGui::Render ();
	}

	template<typename EDITOR_RESOURCES>
	void ImGuiOpenGLEditor<EDITOR_RESOURCES>::EndFrame ()
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