#pragma once

#include "EngineComponents.h"
#include "PxPhysicsAPI.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidBody
	{
		enum class PrimitiveTypes
		{
			BOX,
			SPHERE,
			CAPSULE,
			PLANE
		};

		enum class MeshTypes
		{
			CONVEX,
			TRIANGLE,
			HEIGHT
		};

		PrimitiveTypes m_Primitivetype;
		MeshTypes m_Meshtype;
		int i{ 2 };
		float f{ 3.14f };
		std::string s{ "example" };

		physx::PxPhysics* m_physics{ nullptr };
		//physx::xRigidBody* m_rigidbody{ nullptr };
		
	
	};
	template <>
	inline void NewLuaType<RigidBody>(sol::state& luaState, std::string const& name)
	{
		RegisterLuaType<RigidBody>(luaState, name,
			"i", &RigidBody::i,
			"f", &RigidBody::f,
			"s", &RigidBody::s);
	}
	template<>
	inline void ComponentInspector::operator() < RigidBody > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<RigidBody, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](RigidBody& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("RigidBody");
				ImGui::Separator();

				ImGui::Text("This is a RigidBody component");
				
				ImGui::InputFloat("a float", &component.f);
				ImGui::InputText("a string", &component.s);
			});
	}

	template<>
	inline void JSONify<RigidBody>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidBody& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "i", component.i);
		RapidJSON::JSONifyToValue(value, document, "f", component.f);
		RapidJSON::JSONifyToValue(value, document, "s", component.s);
	}

	template<>
	inline void DeJSONify<RigidBody>(EngineResources& engineResources, RigidBody& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.i, "i");
		AssignIfExist(jsonObj, component.f, "f");
		AssignIfExist(jsonObj, component.s, "s");
	}
}
