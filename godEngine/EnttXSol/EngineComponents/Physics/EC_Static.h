#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidStatic
	{
		physx::PxVec3 Offset;
		bool Active{ true };

			//Non serialize data
			physx::PxRigidStatic* p_RigidStatic;
			bool updateRigidStatic{ true };
			physx::PxScene* mScene;

		//Ctor
		RigidStatic() : Offset{ 0.f, 0.f, 0.f }, p_RigidStatic{ nullptr }, mScene{nullptr}
		{};

		~RigidStatic()
		{
			if (mScene)
			{
				mScene->removeActor(*p_RigidStatic);
				p_RigidStatic->release();
			}

		};
	};
	template <>
	inline void NewLuaType<RigidStatic>(sol::state& luaState, std::string const& name)
	{
	}
	template<>
	inline void ComponentInspector::operator() < RigidStatic > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<RigidStatic, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](RigidStatic& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("RigidStatic Component");
				ImGui::Separator();


				ImGui::InputFloat("Offset x", &component.Offset.x);
				ImGui::InputFloat("Offset y", &component.Offset.y);
				ImGui::InputFloat("Offset z", &component.Offset.z);

				ImGui::Checkbox("Update RigidStatic", &component.updateRigidStatic);

			});
	}
	
	template<>
	inline void JSONify<RigidStatic>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidStatic& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "Offset x", component.Offset.x);
		RapidJSON::JSONifyToValue(value, document, "Offset y", component.Offset.y);
		RapidJSON::JSONifyToValue(value, document, "Offset z", component.Offset.z);
	}

	template<>
	inline void DeJSONify<RigidStatic>(EngineResources& engineResources, RigidStatic& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.Offset.x,"Offset x");
		AssignIfExist(jsonObj, component.Offset.y,"Offset y");
		AssignIfExist(jsonObj, component.Offset.z,"Offset z");


	}

	/*
	*/
}