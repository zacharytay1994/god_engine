#pragma once

#include "EC_Example.h"
#include "EC_EntityData.h"
#include "EC_Transform.h"
#include "EC_Renderable3D.h"
#include "EC_AudioSource.h"
#include "EC_AudioListener.h"
#include "EC_GridCell.h"

#include "Render/EC_PointLight.h"
#include "Render/EC_DirectionalLight.h"
#include "Render/EC_Transparent.h"

#include "Physics/EC_Static.h"
#include "Physics/EC_Dynamic.h"
#include "Physics/EC_PhysicsController.h"

#include "Input/EC_PlayerController.h"

#include "Grid/EC_GridManipulate.h"

#include "GUI/EC_GUIObject.h"
#include "GUI/EC_GUIText.h"

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
			RigidStatic ,
			RigidDynamic ,
			AudioListener ,
			AudioSource ,
			PointLight ,
			DirectionalLight ,
			GridManipulate ,
			GUIObject ,
			GUIText ,
			Transparent ,
			PhysicsController ,
			PlayerController
		>;
		static std::array<std::string , std::tuple_size_v<Components>> m_component_names;
	};

	struct ActiveComponent
	{
		bool m_active { true };
	};
}