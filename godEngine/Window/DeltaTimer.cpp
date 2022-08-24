#include "../pch.h"
#include "DeltaTimer.h"

namespace god
{
	float DeltaTimer::m_dt { 0.0f };
	int DeltaTimer::m_fps { 0 } ;

	void DeltaTimer::StartFrame ()
	{
		m_tp_start_frame = std::chrono::system_clock::now ();

		// interval to record fps, e.g. 1.0f second
		m_time += m_dt;
		if ( m_time > 1.0f )
		{
			m_time = 0.0f;
			m_fps = m_acc_fps;
			m_acc_fps = 0;
		}
		else
		{
			++m_acc_fps;
		}
	}

	void DeltaTimer::EndFrame ()
	{
		m_dt = std::chrono::duration_cast< std::chrono::microseconds >( ( std::chrono::system_clock::now () - m_tp_start_frame ) ).count () / 1'000'000.0f;
	}
}