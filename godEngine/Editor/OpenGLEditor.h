#pragma once
#include "Editor.h"

#include <memory>

namespace god
{
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

		template <template<typename T> class WINDOW>
		void AddWindow ();

		void BeginFrame ();
		void Update ( float dt );
		void Render ();
		void EndFrame ();
	private:
		EDITOR_RESOURCES m_editor_resources;
		std::vector<std::shared_ptr<EditorWindow<EDITOR_RESOURCES>>> m_windows;
	};

	template<typename EDITOR_RESOURCES>
	ImGuiOpenGLEditor<EDITOR_RESOURCES>::ImGuiOpenGLEditor ( EDITOR_RESOURCES editorResources )
		:
		m_editor_resources { editorResources }
	{
		assert ( m_editor_resources.Has<GLFWWindow> () && "ImGuiOpenGLEditor requires an EDITOR_RESOURCES object with a GLFWWindow." );
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
	template <template<typename T> class WINDOW>
	inline void ImGuiOpenGLEditor<EDITOR_RESOURCES>::AddWindow ()
	{
		m_windows.push_back ( std::make_shared<WINDOW<EDITOR_RESOURCES>> () );
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
		for ( auto& window : m_windows )
		{
			window->Update ( dt , m_editor_resources );
		}
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