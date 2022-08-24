#pragma once
#include "../Editor.h"
#include "../../imgui/imgui_stdlib.h" 

namespace god
{
	template <typename EDITOR_RESOURCES>
	struct EW_Performance : EditorWindow<EDITOR_RESOURCES>
	{
		EW_Performance ();
		void Update ( float dt , EDITOR_RESOURCES& engineResources ) override;
	};
}

#include "../../Window/SystemTimer.h"

namespace god
{
	template<typename EDITOR_RESOURCES>
	inline EW_Performance<EDITOR_RESOURCES>::EW_Performance ()
	{
	}

	template<typename EDITOR_RESOURCES>
	inline void EW_Performance<EDITOR_RESOURCES>::Update ( float dt , EDITOR_RESOURCES& engineResources )
	{
		ImGui::Begin ( "Performance View" );

		auto const& time_segments = SystemTimer::GetTimeSegments ();
		float overall = std::get<1> ( time_segments.at ( "Overall" ) );

		float overall_percentage = ( overall / overall ) * 100.0f;
		int i { 0 };
		ImGui::PushID ( i++ );
		ImGui::SliderFloat ( "% Overall" , &overall_percentage , 0.0f , 100.0f );
		ImGui::PopID ();
		for ( auto const& time_segment : time_segments )
		{
			if ( time_segment.first != "Overall" )
			{
				ImGui::PushID ( i++ );
				float percentage = ( std::get<1> ( time_segment.second ) / overall ) * 100.0f;
				ImGui::SliderFloat ( ( std::string ( "% " ) + time_segment.first.c_str () ).c_str () , &percentage , 0.0f , 100.0f );
				ImGui::PopID ();
			}
			++i;
		}

		ImGui::End ();
	}
}