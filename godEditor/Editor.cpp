#include "pch.h"
#include "Editor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"

namespace god
{
	Editor::Editor ( HWND windowHandle )
	{
		ImGui::CreateContext ();

		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplWin32_Init ( windowHandle );
		ImGui_ImplOpenGL3_Init ("#version 430");
	}

	Editor::~Editor ()
	{
		ImGui_ImplOpenGL3_Shutdown ();
		ImGui_ImplWin32_Shutdown ();
		ImGui::DestroyContext ();
	}

	void Editor::Update ()
	{
		// imgui start frame
		ImGui_ImplOpenGL3_NewFrame ();
		ImGui_ImplWin32_NewFrame ();
		ImGui::NewFrame ();

		ImGui::Begin ( "Test" );
		ImGui::Text ( "Hello world" );
		ImGui::End ();

		ImGui::EndFrame ();
		ImGui::Render ();
		ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData () );

		ImGuiIO& io = ImGui::GetIO (); ( void ) io;
		if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		{
			ImGui::UpdatePlatformWindows ();
			ImGui::RenderPlatformWindowsDefault ();
		}
	}
}