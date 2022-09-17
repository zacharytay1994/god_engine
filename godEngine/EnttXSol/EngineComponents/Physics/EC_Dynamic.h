#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidDynamic
	{
		//Dynamic Properties
		physx::PxVec3 AngularVelocity;
		physx::PxVec3 LinearVelocity;
		float Density;

			//Non serialize data
			physx::PxRigidDynamic* p_RigidDynamic;
			bool updateRigidDynamic;

		//Ctor
		RigidDynamic() :  p_RigidDynamic{ nullptr }, updateRigidDynamic{ true },
			AngularVelocity{ 0.f,0.f,0.f }, LinearVelocity{ 0.f,0.f,0.f }, Density{ 1.f }
		{};

		~RigidDynamic()
		{
			if (p_RigidDynamic)
				p_RigidDynamic->release();

		};
};
	template <>
	inline void NewLuaType<RigidDynamic>(sol::state& luaState, std::string const& name)
	{
	}
	template<>
	inline void ComponentInspector::operator() < RigidDynamic > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<RigidDynamic, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](RigidDynamic& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("RigidDynamic Component");
				ImGui::Separator();


	

				ImGui::InputFloat("AngularVelocity x", &component.AngularVelocity.x);
				ImGui::InputFloat("AngularVelocity y", &component.AngularVelocity.y);
				ImGui::InputFloat("AngularVelocity z", &component.AngularVelocity.z);

				ImGui::InputFloat("LinearVelocity x", &component.LinearVelocity.x);
				ImGui::InputFloat("LinearVelocity y", &component.LinearVelocity.y);
				ImGui::InputFloat("LinearVelocity z", &component.LinearVelocity.z);

				ImGui::InputFloat("Density", &component.Density);

				ImGui::Checkbox("Update RigidDynamic", &component.updateRigidDynamic);

			});
	}

	template<>
	inline void JSONify<RigidDynamic>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidDynamic& component)
	{
		(engineResources);
		// serialize

		RapidJSON::JSONifyToValue(value, document, "AngularVelocity x", component.AngularVelocity.x);
		RapidJSON::JSONifyToValue(value, document, "AngularVelocity y", component.AngularVelocity.y);
		RapidJSON::JSONifyToValue(value, document, "AngularVelocity z", component.AngularVelocity.z);

		RapidJSON::JSONifyToValue(value, document, "LinearVelocity x", component.LinearVelocity.x);
		RapidJSON::JSONifyToValue(value, document, "LinearVelocity y", component.LinearVelocity.y);
		RapidJSON::JSONifyToValue(value, document, "LinearVelocity z", component.LinearVelocity.z);

		RapidJSON::JSONifyToValue(value, document, "Density", component.Density);
	}

	template<>
	inline void DeJSONify<RigidDynamic>(EngineResources& engineResources, RigidDynamic& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize


		AssignIfExist(jsonObj, component.AngularVelocity.x, "AngularVelocity x");
		AssignIfExist(jsonObj, component.AngularVelocity.y, "AngularVelocity y");
		AssignIfExist(jsonObj, component.AngularVelocity.z, "AngularVelocity z");

		AssignIfExist(jsonObj, component.LinearVelocity.x, "LinearVelocity x");
		AssignIfExist(jsonObj, component.LinearVelocity.y, "LinearVelocity y");
		AssignIfExist(jsonObj, component.LinearVelocity.z, "LinearVelocity z");

		AssignIfExist(jsonObj, component.Density, "Density");
	}
	/*
	*/
}