#include "../pch.h"
#include "Time.h"

namespace god
{
	float Time::m_dt { 0.0f };

	void Time::StartFrame ()
	{
		m_start_frame_time = std::chrono::system_clock::now ();
	}

	void Time::EndFrame ()
	{
		m_dt = std::chrono::duration_cast< std::chrono::microseconds >( ( std::chrono::system_clock::now () - m_start_frame_time ) ).count () / 1'000'000.0f;
	}
}