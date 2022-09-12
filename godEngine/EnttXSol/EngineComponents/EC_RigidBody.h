#pragma once

#include "EngineComponents.h"
#include "PxPhysicsAPI.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidBody
	{
		enum class Type
		{
			Static,
			Dynamic
		};

		enum class CollisionDetectionType : uint32_t
		{
			Discrete, 
			Continuous,
			ContinuousSpeculative
		};

		Type m_type = Type::Dynamic;
		CollisionDetectionType m_Collisiontype;

		float m_mass = 1.0f;
		float m_LinearDrag = 0.01f;
		float m_AngularDrag = 0.05f;
		bool m_DisableGravity = false;
		bool m_IsKinematic = false;

		uint32_t Layer = 0;
		CollisionDetectionType m_CollisionDetection = CollisionDetectionType::Discrete;

		bool LockPositionX = false;
		bool LockPositionY = false;
		bool LockPositionZ = false;
		bool LockRotationX = false;
		bool LockRotationY = false;
		bool LockRotationZ = false;

		RigidBody() = default;
		RigidBody(const RigidBody& rhs) = default;

		int i{ 2 };
		float f{ 3.14f };
		std::string s{ "example" };

	};

	struct BoxColliderComponent
	{
		glm::vec3 m_size = { 1.0f, 1.0f, 1.0f };
		glm::vec3 m_offset = { 0.0f, 0.0f, 0.0f };
		bool IsTrigger = false;

		// The mesh that will be drawn in the editor to show the collision bounds
		Mesh3D& DebugMesh;

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent& other) = default;
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
