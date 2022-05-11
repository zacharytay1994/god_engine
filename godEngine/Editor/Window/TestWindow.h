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
		ImGui::Begin ( "Test Window" );
		if ( ImGui::Button ( "Activate" ) )
		{
			//std::dynamic_pointer_cast< TestWindow2<EDITOR_RESOURCES> > ( this->Get ( typeid( TestWindow2 ).name () ) )->test = true;
			this->Get<TestWindow2> ()->test = true;
			//Get<TestWindow2> ()->test = true;
		}
		if ( ImGui::Button ( "Activate2" ) )
		{
			//std::dynamic_pointer_cast< TestWindow2<EDITOR_RESOURCES> > ( this->Get ( typeid( TestWindow2 ).name () ) )->test = false;
			//Get<TestWindow2> ()->test = true;
			this->Get<TestWindow2> ()->test = false;
		}
		if ( test )
		{
			ImGui::Text ( "Test from window 2" );
		}
		ImGui::End ();
	}
}