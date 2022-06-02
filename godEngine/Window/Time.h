#pragma once

#include <chrono>
#include <ctime>

namespace god
{
	struct Time
	{
		static float m_dt;
		std::chrono::system_clock::time_point m_start_frame_time;

		void StartFrame ();
		void EndFrame ();
	};
}