#pragma once
#include "../Editor.h"

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_MainMenuBar : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	};
}

#include "EW_EditorStyles.h"
#include "EW_AssetImporter.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline void EW_MainMenuBar<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		if ( ImGui::BeginMainMenuBar () )
		{
			if ( ImGui::BeginMenu ( "File" ) )
			{
				if ( ImGui::BeginMenu ( "New" ) )
				{
					if ( ImGui::MenuItem ( "Asset" , "[NONE]" ) )
					{
						this->Get<EW_Asset3DImporter> ()->m_open = true;
					}

					ImGui::EndMenu ();
				}

				ImGui::EndMenu ();
			}

			if ( ImGui::BeginMenu ( "View" ) )
			{
				if ( ImGui::MenuItem ( "Editor Style", "[NONE]") )
				{
					this->Get<EW_EditorStyles> ()->m_open = true;
				}

				if ( ImGui::MenuItem ( "Asset Manager" , "[NONE]" ) )
				{
					//this->Get<EW_AssetManager> ()->m_open = true;
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