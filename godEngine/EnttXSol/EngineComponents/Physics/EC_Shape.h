#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct PhysicsShape
	{

		//eSPHERE,
		//	ePLANE,
		//	eCAPSULE,
		//	eBOX,
		physx::PxVec3 extents;
		
			//Non serialize data
			physx::PxShape* p_shape;
			bool updatePhysicsShape;

		//Ctor
		PhysicsShape() : extents{ physx::PxVec3(20.f,20.f,20.f) }, p_shape{ nullptr }, updatePhysicsShape{ true }
		{};
		~PhysicsShape() 
		{
			//if(p_shape)
			//	p_shape->release();
		};
	};
	template <>
	inline void NewLuaType<PhysicsShape>(sol::state& luaState, std::string const& name)
	{
		UNREFERENCED_PARAMETER(luaState);
		UNREFERENCED_PARAMETER(name);
	}
	template<>
	inline void ComponentInspector::operator() < PhysicsShape > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<PhysicsShape, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](PhysicsShape& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("PhysicsShape Component");
				ImGui::Separator();

				ImGui::InputFloat("x_extent", &component.extents.x);
				ImGui::InputFloat("y_extent", &component.extents.y);
				ImGui::InputFloat("z_extent", &component.extents.z);

				ImGui::Checkbox("Update Shape", &component.updatePhysicsShape);


			});
	}
	
	template<>
	inline void JSONify<PhysicsShape>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, PhysicsShape& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "x extent", component.extents.x);
		RapidJSON::JSONifyToValue(value, document, "y extent", component.extents.y);
		RapidJSON::JSONifyToValue(value, document, "z extent", component.extents.z);
	}

	template<>
	inline void DeJSONify<PhysicsShape>(EngineResources& engineResources, PhysicsShape& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.extents.x, "x extent");
		AssignIfExist(jsonObj, component.extents.y, "y extent");
		AssignIfExist(jsonObj, component.extents.z, "z extent");
	}

	/*
	*/


}