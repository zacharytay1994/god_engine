#pragma once

#include <chrono>
#include <ctime>

namespace god
{
	struct DeltaTimer
	{
		static float m_dt;
		static int m_fps;
		static float m_acc_dt;

		void StartFrame ();
		void EndFrame ();

	private:
		float m_time { 0.0f };
		int m_acc_fps { 0 };
		std::chrono::system_clock::time_point m_tp_start_frame;
	};
}