#pragma once

#include "EC_Example.h"
#include "EC_EntityData.h"
#include "EC_Transform.h"
#include "EC_Renderable3D.h"
#include "EC_AudioSource.h"
#include "EC_AudioListener.h"
#include "EC_GridCell.h"

#include "Render/EC_PointLight.h"

#include "Physics/EC_Material.h"
#include "Physics/EC_Shape.h"
#include "Physics/EC_Static.h"
#include "Physics/EC_Dynamic.h"

#include <tuple>
#include <string>
#include <array>

namespace god
{
	struct EngineComponents
	{
		using Components = std::tuple<
			EntityData ,
			Transform ,
			Renderable3D ,
			GridCell ,
			PhysicsMaterial ,
			PhysicsShape ,
			RigidStatic ,
			RigidDynamic ,
			AudioListener ,
			AudioSource ,
			PointLight
		>;
		static std::array<std::string , std::tuple_size_v<Components>> m_component_names;
	};
}