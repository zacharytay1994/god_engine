#pragma once

#include <chrono>
#include <ctime>
#include <tuple>
#include <unordered_map>
#include <string>

namespace god
{
	struct SystemTimer
	{
		using TimePoint = std::chrono::system_clock::time_point;
		using TimeSegment = std::tuple<TimePoint , float>;
		using TimeSegments = std::unordered_map<std::string , TimeSegment>;

		static void StartTimeSegment ( std::string const& name );
		static void EndTimeSegment ( std::string const& name );

		static TimeSegments const& GetTimeSegments ();

	private:
		static TimeSegments m_time_segments;
	};
}