#pragma once

#include "../EngineComponents.h"
//#include "../../Physics/godPhysics.h"
#include "../../godEngine/Physics/Types.h"


namespace god
{
	/* ENGINE COMPONENTS */
	struct RigidDynamic
	{
		//Material
		float StaticFriction, DynamicFriction, Restitution;
		//shape
		physx::PxVec3 extents;
		bool locktoscale;
		//PhysicsTypes IDs
		int Shapeid;	// Cube=0, Sphere, Capsule, TriangleMesh
		int PhysicsTypeid;	//Dice = 0, Player, Enemy, Bullet, Tile
		//Dynamic Properties
		physx::PxVec3 AngularVelocity;
		physx::PxVec3 LinearVelocity;
		float Density;

		/* Actor Flags*/
		bool Active{ true };
		bool Gravity{ true };
		/* Shape Flags*/
		bool Simulation{ true };
		bool Trigger{ false };
		
		//Non serialize data
		uint32_t m_id;
		physx::PxTriangleMesh* p_trimesh;
		physx::PxMaterial* p_material;
		physx::PxShape* p_shape;	
		physx::PxRigidDynamic* p_RigidDynamic;

		bool updateRigidDynamic;
		physx::PxScene* mScene;

		//Ctor
		RigidDynamic() :  
			p_material{ nullptr }, p_RigidDynamic{ nullptr }, mScene{ nullptr }, p_trimesh{nullptr},
			Shapeid{ 0 }, PhysicsTypeid{ 0 }, extents{ physx::PxVec3(20.f, 20.f, 20.f) }, p_shape{ nullptr },
			locktoscale{ true }, Active{ true }, Simulation{ true }, Trigger{ false }, Gravity{ true },
			StaticFriction{ 0.5f }, DynamicFriction{ 0.5f }, Restitution{ 0.5f }, 
			AngularVelocity{ 0.f,0.f,0.f }, LinearVelocity{ 0.f,0.f,0.f }, Density{ 1.f }, updateRigidDynamic{ true }
		{};

		~RigidDynamic()
		{
			if (mScene)
			{
				mScene->removeActor(*p_RigidDynamic);
			}

		};
	};
	template <>
	inline void NewLuaType<RigidDynamic>(sol::state& luaState, std::string const& name)
	{
		UNREFERENCED_PARAMETER(luaState);
		UNREFERENCED_PARAMETER(name);
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

				ImGui::Checkbox("Active", &component.Active);

				int selected_type = component.PhysicsTypeid;
				const char* typenames[] = { "Default", "Dice", "Player", "Enemy", "Bullet", "Tile" };
				char buf1[64];
				sprintf_s(buf1, "%s###", typenames[selected_type]); // ### operator override ID ignoring the preceding label
				ImGui::Text("Physics Type :");
				if (ImGui::Button(buf1))
				{
				
					ImGui::OpenPopup("type popup");
				}
				//ImGui::TextUnformatted( names[selected_shape] );
				if (ImGui::BeginPopup("type popup"))
				{
					for (int i = 0; i < IM_ARRAYSIZE(typenames); i++)
						if (ImGui::Selectable(typenames[i]))
						{
							selected_type = i;
							component.PhysicsTypeid = i;
							component.updateRigidDynamic = true;
							ImGui::CloseCurrentPopup();
						}
					ImGui::EndPopup();
				}



				ImGui::InputFloat("StaticFriction", &component.StaticFriction);
				ImGui::InputFloat("DynamicFriction", &component.DynamicFriction);
				ImGui::InputFloat("Restitution", &component.Restitution);


				int selected_shape = component.Shapeid;
				const char* names[] = { "Cube", "Sphere", "Capsule" };
				char buf[64];
				sprintf_s(buf, "%s###", names[selected_shape]); // ### operator override ID ignoring the preceding label
				ImGui::Text("Shape Type :");
				ImGui::PushID(1);
				if (ImGui::Button(buf))
				{
					ImGui::OpenPopup("shape popup");
					ImGui::PopID();
				}
				else
				{
					ImGui::PopID();
				}
				if (ImGui::BeginPopup("shape popup"))
				{

					for (int i = 0; i < IM_ARRAYSIZE(names); i++)
						if (ImGui::Selectable(names[i]))
						{
							selected_shape = i;
							component.Shapeid = i;
							component.updateRigidDynamic = true;
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
	
				ImGui::Checkbox("Gravity", &component.Gravity);
				ImGui::Checkbox("Trigger", &component.Trigger);
				ImGui::Checkbox("Simulation", &component.Simulation);
				ImGui::Checkbox("Lock Extents to Scale", &component.locktoscale);
				ImGui::Separator();


				ImGui::InputFloat("AngularVelocity x", &component.AngularVelocity.x);
				ImGui::InputFloat("AngularVelocity y", &component.AngularVelocity.y);
				ImGui::InputFloat("AngularVelocity z", &component.AngularVelocity.z);

				ImGui::InputFloat("LinearVelocity x", &component.LinearVelocity.x);
				ImGui::InputFloat("LinearVelocity y", &component.LinearVelocity.y);
				ImGui::InputFloat("LinearVelocity z", &component.LinearVelocity.z);

				ImGui::InputFloat("Density", &component.Density);


				ImGui::Separator();
			});
	}

	template<>
	inline void JSONify<RigidDynamic>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidDynamic& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "Active", component.Active);


		RapidJSON::JSONifyToValue(value, document, "StaticFriction", component.StaticFriction);
		RapidJSON::JSONifyToValue(value, document, "DynamicFriction", component.DynamicFriction);
		RapidJSON::JSONifyToValue(value, document, "Restitution", component.Restitution);

		RapidJSON::JSONifyToValue(value, document, "x extent", component.extents.x);
		RapidJSON::JSONifyToValue(value, document, "y extent", component.extents.y);
		RapidJSON::JSONifyToValue(value, document, "z extent", component.extents.z);

		RapidJSON::JSONifyToValue(value, document, "Simulation", component.Simulation);
		RapidJSON::JSONifyToValue(value, document, "Gravity", component.Gravity);
		RapidJSON::JSONifyToValue(value, document, "Trigger", component.Trigger);

		RapidJSON::JSONifyToValue(value, document, "scale lock", component.locktoscale);
		RapidJSON::JSONifyToValue(value, document, "shapeid", component.Shapeid);
		RapidJSON::JSONifyToValue(value, document, "PhysicsTypeid", component.PhysicsTypeid);

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
		AssignIfExist(jsonObj, component.Active, "Active");

		AssignIfExist(jsonObj, component.StaticFriction, "StaticFriction");
		AssignIfExist(jsonObj, component.DynamicFriction, "DynamicFriction");
		AssignIfExist(jsonObj, component.Restitution, "Restitution");

		AssignIfExist(jsonObj, component.extents.x, "x extent");
		AssignIfExist(jsonObj, component.extents.y, "y extent");
		AssignIfExist(jsonObj, component.extents.z, "z extent");
		
		AssignIfExist(jsonObj, component.Simulation, "Simulation");
		AssignIfExist(jsonObj, component.Gravity, "Gravity");
		AssignIfExist(jsonObj, component.Trigger, "Trigger");

		AssignIfExist(jsonObj, component.locktoscale, "scale lock");
		AssignIfExist(jsonObj, component.Shapeid, "shapeid");
		AssignIfExist(jsonObj, component.PhysicsTypeid, "PhysicsTypeid");
		
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