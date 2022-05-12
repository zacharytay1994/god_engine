#pragma once
#include "../Editor.h"

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct MainMenuBar : EditorWindow<EDITOR_RESOURCES>
	{
		bool popup { false };

		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	};
}

#include "TestWindow.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void MainMenuBar<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		if ( ImGui::BeginMainMenuBar () )
		{
			if ( ImGui::BeginMenu ( "File" ) )
			{
				if ( ImGui::BeginMenu ( "New" ) )
				{
					ImGui::MenuItem ( "Texture" , "[CTRL+N+T]" );
					ImGui::EndMenu ();
				}

				ImGui::EndMenu ();
			}

			if ( ImGui::BeginMenu ( "Edit" ) )
			{

				ImGui::EndMenu ();
			}

			ImGui::EndMainMenuBar ();
		}
	}
}