#pragma once
#include "../Editor.h"
#include "../../imgui/imgui_stdlib.h" 
#include <sstream>
#include <algorithm>

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
		( dt );
		( engineResources );
		ImGui::Begin ( "Performance View" );
		ImGui::Text ( "Time in microseconds. i.e. 1/1,000,000 seconds" );
		auto const& time_segments = SystemTimer::GetTimeSegments ();

		std::vector<std::pair<std::string , SystemTimer::TimeSegment>> sorted_time_segments;
		std::copy ( time_segments.begin () , time_segments.end () , std::back_inserter ( sorted_time_segments ) );
		std::sort ( sorted_time_segments.begin () , sorted_time_segments.end () ,
			[]( auto const& t1 , auto const& t2 )
			{
				return std::get<1> ( std::get<1> ( t1 ) ) > std::get<1> ( std::get<1> ( t2 ) );
			} );

		float overall = std::get<1> ( time_segments.at ( "Overall" ) );

		float overall_percentage = ( overall / overall ) * 100.0f;
		int i { 0 };
		ImGui::PushID ( i++ );
		ImGui::SliderFloat ( "% Overall" , &overall_percentage , 0.0f , 100.0f );
		ImGui::PopID ();
		for ( auto const& time_segment : sorted_time_segments )
		{
			if ( time_segment.first != "Overall" )
			{
				ImGui::PushID ( i++ );
				float percentage = ( std::get<1> ( time_segment.second ) / overall ) * 100.0f;
				std::stringstream ss;
				ss << "% [" << static_cast< int >( std::get<1> ( time_segment.second ) * 1'000'000.0f ) << "] " << time_segment.first;
				ImGui::SliderFloat ( ss.str ().c_str () , &percentage , 0.0f , 100.0f );
				ImGui::PopID ();
			}
			++i;
		}

		ImGui::End ();
	}
}