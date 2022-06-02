#include "../../pch.h"
#include "CommonSystems.h"

#include "../../Window/Time.h"

namespace god
{
	void UpdatePosition ( Position3D& position , Velocity3D& velocity )
	{
		position += velocity * Time::m_dt;
	}
}
