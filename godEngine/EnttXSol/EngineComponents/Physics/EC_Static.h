#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidStatic
	{
		//Material
		float StaticFriction, DynamicFriction, Restitution;
		//shape
		physx::PxVec3 extents;
		bool locktoscale;
		//PhysicsTypes IDs
		int Shapeid;	//Default = 0 = Cube
		int PhysicsTypeid;	//Default = 0
		//Offset
		physx::PxVec3 Offset;

		/* Actor Flags*/
		bool Active{ true };
		
		/* Shape Flags*/
		bool Simulation{ true };
		bool Trigger{ false };

		//Non serialize data
		physx::PxMaterial* p_material{ nullptr };
		physx::PxShape* p_shape;

		physx::PxRigidStatic* p_RigidStatic;
		bool updateRigidStatic{ true };
		physx::PxScene* mScene;

		//Ctor
		RigidStatic() :  StaticFriction{ 0.5f }, DynamicFriction{ 0.5f }, Restitution{ 0.5f }, p_material{ nullptr }, 
			Shapeid{ 0 }, extents{ physx::PxVec3(20.f, 20.f, 20.f) }, locktoscale{ true }, Offset{ 0.f, 0.f, 0.f },
			Active{ true },  Simulation{ true },  Trigger{ false }, p_shape{ nullptr }, p_RigidStatic{ nullptr }, mScene{nullptr}
		{};

		~RigidStatic()
		{
			if (mScene)
			{
				mScene->removeActor(*p_RigidStatic);
			}

		};
	};
	template <>
	inline void NewLuaType<RigidStatic>(sol::state& luaState, std::string const& name)
	{
		UNREFERENCED_PARAMETER(luaState);
		UNREFERENCED_PARAMETER(name);
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
				ImGui::Checkbox("Active", &component.Active);
				ImGui::InputFloat("StaticFriction", &component.StaticFriction);
				ImGui::InputFloat("DynamicFriction", &component.DynamicFriction);
				ImGui::InputFloat("Restitution", &component.Restitution);

				int selected_shape = component.Shapeid;
				const char* names[] = { "Cube", "Sphere", "Capsule" };
				char buf[64];
				sprintf_s(buf, "%s###", names[selected_shape]); // ### operator override ID ignoring the preceding label
				if (ImGui::Button(buf))
				{
					ImGui::OpenPopup("shape popup");
				}
				//ImGui::TextUnformatted( names[selected_shape] );
				if (ImGui::BeginPopup("shape popup"))
				{
					ImGui::Text("Shape type");
					ImGui::Separator();
					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						if (ImGui::Selectable(names[i]))
						{
							selected_shape = i;
							component.Shapeid = i;
							ImGui::CloseCurrentPopup();
						}
					ImGui::EndPopup();
				}

				switch (component.Shapeid)
				{
				case PhysicsTypes::Cube:
					ImGui::InputFloat("X Extent", &component.extents.x);
					ImGui::InputFloat("Y Extent", &component.extents.y);
					ImGui::InputFloat("Z Extent", &component.extents.z);
					break;
				case PhysicsTypes::Sphere:
					ImGui::InputFloat("Radius", &component.extents.x);
					break;
				case PhysicsTypes::Capsule:
					ImGui::InputFloat("Radius", &component.extents.x);
					ImGui::InputFloat("Half Height", &component.extents.y);
					break;

					
				}

				ImGui::Separator();
				ImGui::Text("Shape Flags");

				ImGui::Checkbox("Trigger", &component.Trigger);
				ImGui::Checkbox("Simulation", &component.Simulation);
				ImGui::Separator();
			
				ImGui::Checkbox("Lock Extents to Scale", &component.locktoscale);

				ImGui::InputFloat("Offset x", &component.Offset.x);
				ImGui::InputFloat("Offset y", &component.Offset.y);
				ImGui::InputFloat("Offset z", &component.Offset.z);
				ImGui::Separator();


			});
	}
	
	template<>
	inline void JSONify<RigidStatic>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidStatic& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "StaticFriction", component.StaticFriction);
		RapidJSON::JSONifyToValue(value, document, "DynamicFriction", component.DynamicFriction);
		RapidJSON::JSONifyToValue(value, document, "Restitution", component.Restitution);

		RapidJSON::JSONifyToValue(value, document, "x extent", component.extents.x);
		RapidJSON::JSONifyToValue(value, document, "y extent", component.extents.y);
		RapidJSON::JSONifyToValue(value, document, "z extent", component.extents.z);

		RapidJSON::JSONifyToValue(value, document, "Simulation", component.Simulation);
		RapidJSON::JSONifyToValue(value, document, "Trigger", component.Trigger);

		RapidJSON::JSONifyToValue(value, document, "scale lock", component.locktoscale);
		RapidJSON::JSONifyToValue(value, document, "shapeid", component.Shapeid);
		RapidJSON::JSONifyToValue(value, document, "PhysicsTypeid", component.PhysicsTypeid);

		RapidJSON::JSONifyToValue(value, document, "Offset x", component.Offset.x);
		RapidJSON::JSONifyToValue(value, document, "Offset y", component.Offset.y);
		RapidJSON::JSONifyToValue(value, document, "Offset z", component.Offset.z);
	}

	template<>
	inline void DeJSONify<RigidStatic>(EngineResources& engineResources, RigidStatic& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.StaticFriction, "StaticFriction");
		AssignIfExist(jsonObj, component.DynamicFriction, "DynamicFriction");
		AssignIfExist(jsonObj, component.Restitution, "Restitution");

		AssignIfExist(jsonObj, component.extents.x, "x extent");
		AssignIfExist(jsonObj, component.extents.y, "y extent");
		AssignIfExist(jsonObj, component.extents.z, "z extent");

		AssignIfExist(jsonObj, component.Simulation, "Simulation");
		AssignIfExist(jsonObj, component.Trigger, "Trigger");

		AssignIfExist(jsonObj, component.locktoscale, "scale lock");
		AssignIfExist(jsonObj, component.Shapeid, "shapeid");
		AssignIfExist(jsonObj, component.PhysicsTypeid, "PhysicsTypeid");

		AssignIfExist(jsonObj, component.Offset.x,"Offset x");
		AssignIfExist(jsonObj, component.Offset.y,"Offset y");
		AssignIfExist(jsonObj, component.Offset.z,"Offset z");


	}

	/*
	*/
}