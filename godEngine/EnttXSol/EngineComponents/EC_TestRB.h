#pragma once

#include "EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct TestRB
	{
		float m_matx, m_maty, m_matz;
		physx::PxMaterial* m_material{ nullptr };
		float m_shapex, m_shapey, m_shapez;
		physx::PxShape* m_shape{ nullptr };
	};
	template <>
	inline void NewLuaType<TestRB>(sol::state& luaState, std::string const& name)
	{
	}
	template<>
	inline void ComponentInspector::operator() < TestRB > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<TestRB, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](TestRB& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("Example Component");
				ImGui::Separator();
			});
	}

	template<>
	inline void JSONify<TestRB>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, TestRB& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "matx", component.m_matx);
		RapidJSON::JSONifyToValue(value, document, "maty", component.m_maty);
		RapidJSON::JSONifyToValue(value, document, "matz", component.m_matz);
	}

	template<>
	inline void DeJSONify<TestRB>(EngineResources& engineResources, TestRB& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.m_matx, "matx");
		AssignIfExist(jsonObj, component.m_maty, "maty");
		AssignIfExist(jsonObj, component.m_matz, "matz");

		engineResources.Get<PhysicsSystem>().get().GetPhysics()->createMaterial(component.m_matx, component.m_maty, component.m_matz);
	}
}