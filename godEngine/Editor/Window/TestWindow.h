#pragma once
#include "../Editor.h"

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct TestWindow : EditorWindow<EDITOR_RESOURCES>
	{
		bool test { false };

		void Update ( float dt , EDITOR_RESOURCES& editorResources ) override;
	};
}

#include "TestWindow2.h"

namespace god 
{
	template<typename EDITOR_RESOURCES>
	inline void TestWindow<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& editorResources )
	{
		UNREFERENCED_PARAMETER ( dt );
		UNREFERENCED_PARAMETER ( editorResources );

		ImGui::Begin ( "Test Window" );
		if ( ImGui::Button ( "Activate" ) )
		{
			this->Get<TestWindow2> ()->test = true;
		}
		if ( ImGui::Button ( "Activate2" ) )
		{
			this->Get<TestWindow2> ()->test = false;
		}
		if ( test )
		{
			ImGui::Text ( "Test from window 2" );
		}
		ImGui::End ();
	}
}