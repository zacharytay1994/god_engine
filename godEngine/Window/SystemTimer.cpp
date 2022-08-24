#include "../pch.h"
#include "SystemTimer.h"
#include "DeltaTimer.h"

#include <cmath>

namespace god
{
	SystemTimer::TimeSegments SystemTimer::m_time_segments;

	void SystemTimer::StartTimeSegment ( std::string const& name )
	{
		std::get<0> ( m_time_segments[ name ] ) = std::chrono::system_clock::now ();
	}

	void SystemTimer::EndTimeSegment ( std::string const& name )
	{
		auto current = std::get<1> ( m_time_segments[ name ] );
		std::get<1> ( m_time_segments[ name ] ) =
			std::lerp ( 
				current ,
				std::chrono::duration_cast< std::chrono::microseconds >(
					std::chrono::system_clock::now () - std::get<0> ( m_time_segments[ name ] ) ).count () / 1'000'000.0f ,
				DeltaTimer::m_dt * 0.2f
			);
	}

	SystemTimer::TimeSegments const& SystemTimer::GetTimeSegments ()
	{
		return m_time_segments;
	}
}