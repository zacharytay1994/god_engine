#include "../../pch.h"
#include "EC_All.h"

namespace god
{
	std::array<std::string , std::tuple_size_v<EngineComponents::Components>> EngineComponents::m_component_names
	{
		"EntityData",
		"Transform",
		"Renderable3D",
		"GridCell",
		"RigidStatic",
		"RigidDynamic",
		"AudioListener",
		"AudioSource" ,
		"PointLight" ,
		"DirectionalLight" ,
		"GridManipulate" ,
		"GUIObject" ,
		"GUIText" ,
		"Transparent",
		"PhysicsController",
		"PlayerController",
		"SkeleAnim3D"
	};
}