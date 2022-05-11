#pragma once
#include "../Editor.h"

#include "../../Window/GLFWWindow.h"

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct TestWindow2 : EditorWindow<EDITOR_RESOURCES>
	{
		bool test { false };

		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	};
}

#include "TestWindow.h"

namespace god 
{
	template<typename EDITOR_RESOURCES>
	inline void TestWindow2<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		ImGui::Begin ( "Test Window2" );
		if ( test )
		{
			ImGui::Text ( "%d" , editorResources.Get<GLFWWindow> ().get ().MouseX () );
		}
		ImGui::Text ( "testing2" );
		if ( ImGui::Button ( "TestWindow1" ) )
		{
			this->Get<TestWindow> ()->test = true;
		}
		ImGui::End ();
	}
}