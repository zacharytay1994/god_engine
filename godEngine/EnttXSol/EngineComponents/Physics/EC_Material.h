#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct PhysicsMaterial
	{
		float StaticFriction, DynamicFriction, Restitution;

			//Non serialize data
			physx::PxMaterial* p_material{ nullptr };
			bool updatematerial;

		//Ctor
		PhysicsMaterial() : StaticFriction{ 0.5f }, DynamicFriction{ 0.5f }, Restitution{ 0.5f }, p_material{ nullptr }, updatematerial{ true }
		{};
		~PhysicsMaterial()
		{
			if (p_material)
				p_material->release();
		};
	};
	template <>
	inline void NewLuaType<PhysicsMaterial>(sol::state& luaState, std::string const& name)
	{
	}
	template<>
	inline void ComponentInspector::operator() < PhysicsMaterial > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<PhysicsMaterial, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](PhysicsMaterial& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("PhysicsMaterial Component");
				ImGui::Separator();

		
				ImGui::InputFloat("StaticFriction", &component.StaticFriction);
				ImGui::InputFloat("DynamicFriction", &component.DynamicFriction);
				ImGui::InputFloat("Restitution", &component.Restitution);

				ImGui::Checkbox("Update Material", &component.updatematerial);
			
			});
	}
	
	template<>
	inline void JSONify<PhysicsMaterial>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, PhysicsMaterial& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "StaticFriction", component.StaticFriction);
		RapidJSON::JSONifyToValue(value, document, "DynamicFriction", component.DynamicFriction);
		RapidJSON::JSONifyToValue(value, document, "Restitution", component.Restitution);
	}

	template<>
	inline void DeJSONify<PhysicsMaterial>(EngineResources& engineResources, PhysicsMaterial& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.StaticFriction, "StaticFriction");
		AssignIfExist(jsonObj, component.DynamicFriction, "DynamicFriction");
		AssignIfExist(jsonObj, component.Restitution, "Restitution");

		
	}

	/*
	*/
}