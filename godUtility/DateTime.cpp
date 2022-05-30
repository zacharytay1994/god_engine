#include "pch.h"
#include "DateTime.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace god
{
	std::string GetDateTimeString ()
	{
		auto time = std::time ( nullptr );
		struct tm buf;
		localtime_s ( &buf , &time );
		std::ostringstream oss;
		oss << std::put_time ( &buf , "%d-%m-%Y %H-%M-%S" );
		return oss.str ();
	}
}