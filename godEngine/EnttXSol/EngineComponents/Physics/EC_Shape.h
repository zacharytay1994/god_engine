#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct PhysicsShape
	{

		//	eSPHERE,
		//	ePLANE,
		//	eCAPSULE,
		//	eBOX,
		physx::PxVec3 extents;
		bool locktoscale;
		enum shapetype 
		{
			Cube, Sphere, Capsule, Plane
		};
		int shapeid;	


			//Non serialize data
			physx::PxShape* p_shape;
			bool updatePhysicsShape;

		//Ctor
			PhysicsShape() :shapeid{ 0 }, extents { physx::PxVec3(20.f, 20.f, 20.f) }, p_shape{ nullptr }, updatePhysicsShape{ true }, locktoscale{ true }
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


				int selected_shape = component.shapeid;
				const char* names[] = { "Cube", "Sphere", "Capsule","Plane"};
				char buf[64];
				sprintf_s(buf, "%s###", names[selected_shape]); // ### operator override ID ignoring the preceding label
				if (ImGui::Button(buf))
				{
					ImGui::OpenPopup("shape popup");
				}
				//ImGui::TextUnformatted( names[selected_shape] );
				if (ImGui::BeginPopup("shape popup"))
				{
					//ImGui::Text("Shape type");
					//ImGui::Separator();
					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						if (ImGui::Selectable(names[i]))
						{
							selected_shape = i;
							component.shapeid = i;
							ImGui::CloseCurrentPopup();
						}
					ImGui::EndPopup();
				}

				switch (component.shapeid)
				{
					case PhysicsShape::Cube	:
						ImGui::InputFloat("X Extent", &component.extents.x);
						ImGui::InputFloat("Y Extent", &component.extents.y);
						ImGui::InputFloat("Z Extent", &component.extents.z);
						break;
					case PhysicsShape::Sphere:
						ImGui::InputFloat("Radius", &component.extents.x);
						break;
					case PhysicsShape::Capsule:
						ImGui::InputFloat("Radius", &component.extents.x);
						ImGui::InputFloat("Half Height", &component.extents.y);
						break;
					case PhysicsShape::Plane:
						ImGui::InputFloat("X Normal", &component.extents.x);
						ImGui::InputFloat("Y Normal", &component.extents.y);
						ImGui::InputFloat("Z Normal", &component.extents.z);
						break;
				}


				ImGui::Checkbox("Lock Extents to Scale", &component.locktoscale);

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
		RapidJSON::JSONifyToValue(value, document, "scale lock", component.locktoscale);
		RapidJSON::JSONifyToValue(value, document, "shapeid", component.shapeid);
	}

	template<>
	inline void DeJSONify<PhysicsShape>(EngineResources& engineResources, PhysicsShape& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.extents.x, "x extent");
		AssignIfExist(jsonObj, component.extents.y, "y extent");
		AssignIfExist(jsonObj, component.extents.z, "z extent");
		AssignIfExist(jsonObj, component.locktoscale, "scale lock");
		AssignIfExist(jsonObj, component.shapeid, "shapeid");
	}

	/*
	*/


}