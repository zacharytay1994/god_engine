#pragma once

#include "../Editor.h"

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct TestWindow : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override
		{
			ImGui::Begin ( "Test Window" );
			ImGui::Text ( "testing" );
			ImGui::End ();
		}
	};


	template <typename EDITOR_RESOURCES>
	struct TestWindow2 : EditorWindow<EDITOR_RESOURCES>
	{
		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override
		{
			ImGui::Begin ( "Test Window2" );
			ImGui::Text ( "testing2" );
			ImGui::End ();
		}
	};
}