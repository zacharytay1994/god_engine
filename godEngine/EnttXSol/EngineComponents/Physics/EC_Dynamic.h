#pragma once

#include "../EngineComponents.h"
#include "../../Physics/godPhysics.h"

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
		enum shapetype
		{
			Cube, Sphere, Capsule, TriangleMesh
		};
		int shapeid;
		//Dynamic Properties
		physx::PxVec3 AngularVelocity;
		physx::PxVec3 LinearVelocity;
		float Density;
		bool Active{ true };


		//Non serialize data
		physx::PxTriangleMesh* p_trimesh;
		physx::PxMaterial* p_material;
		physx::PxShape* p_shape;	
		physx::PxRigidDynamic* p_RigidDynamic;

		bool updateRigidDynamic{ true };
		physx::PxScene* mScene;

		//Ctor
		RigidDynamic() :  
			p_material{ nullptr }, p_RigidDynamic{ nullptr }, mScene{ nullptr }, p_trimesh{nullptr},
			shapeid{ 0 }, extents{ physx::PxVec3(20.f, 20.f, 20.f) }, p_shape{ nullptr },  locktoscale{ true },
			StaticFriction{ 0.5f }, DynamicFriction{ 0.5f }, Restitution{ 0.5f }, 
			AngularVelocity{ 0.f,0.f,0.f }, LinearVelocity{ 0.f,0.f,0.f }, Density{ 1.f }
		{};

		~RigidDynamic()
		{
			if (mScene)
			{
				mScene->removeActor(*p_RigidDynamic);
				//p_RigidDynamic->release();
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


				ImGui::InputFloat("StaticFriction", &component.StaticFriction);
				ImGui::InputFloat("DynamicFriction", &component.DynamicFriction);
				ImGui::InputFloat("Restitution", &component.Restitution);


				int selected_shape = component.shapeid;
				const char* names[] = { "Cube", "Sphere", "Capsule","Triangle Mesh" };
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
				case RigidDynamic::Cube:
					ImGui::InputFloat("X Extent", &component.extents.x);
					ImGui::InputFloat("Y Extent", &component.extents.y);
					ImGui::InputFloat("Z Extent", &component.extents.z);
					break;
				case RigidDynamic::Sphere:
					ImGui::InputFloat("Radius", &component.extents.x);
					break;
				case RigidDynamic::Capsule:
					ImGui::InputFloat("Radius", &component.extents.x);
					ImGui::InputFloat("Half Height", &component.extents.y);
					break;
				
				}


				ImGui::Checkbox("Lock Extents to Scale", &component.locktoscale);



				ImGui::InputFloat("AngularVelocity x", &component.AngularVelocity.x);
				ImGui::InputFloat("AngularVelocity y", &component.AngularVelocity.y);
				ImGui::InputFloat("AngularVelocity z", &component.AngularVelocity.z);

				ImGui::InputFloat("LinearVelocity x", &component.LinearVelocity.x);
				ImGui::InputFloat("LinearVelocity y", &component.LinearVelocity.y);
				ImGui::InputFloat("LinearVelocity z", &component.LinearVelocity.z);

				ImGui::InputFloat("Density", &component.Density);



			});
	}

	template<>
	inline void JSONify<RigidDynamic>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, RigidDynamic& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "StaticFriction", component.StaticFriction);
		RapidJSON::JSONifyToValue(value, document, "DynamicFriction", component.DynamicFriction);
		RapidJSON::JSONifyToValue(value, document, "Restitution", component.Restitution);

		RapidJSON::JSONifyToValue(value, document, "x extent", component.extents.x);
		RapidJSON::JSONifyToValue(value, document, "y extent", component.extents.y);
		RapidJSON::JSONifyToValue(value, document, "z extent", component.extents.z);
		RapidJSON::JSONifyToValue(value, document, "scale lock", component.locktoscale);
		RapidJSON::JSONifyToValue(value, document, "shapeid", component.shapeid);


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

		AssignIfExist(jsonObj, component.StaticFriction, "StaticFriction");
		AssignIfExist(jsonObj, component.DynamicFriction, "DynamicFriction");
		AssignIfExist(jsonObj, component.Restitution, "Restitution");

		AssignIfExist(jsonObj, component.extents.x, "x extent");
		AssignIfExist(jsonObj, component.extents.y, "y extent");
		AssignIfExist(jsonObj, component.extents.z, "z extent");
		AssignIfExist(jsonObj, component.locktoscale, "scale lock");
		AssignIfExist(jsonObj, component.shapeid, "shapeid");


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