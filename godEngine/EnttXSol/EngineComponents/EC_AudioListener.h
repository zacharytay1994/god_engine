#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct AudioListener
	{
		int m_listener_id{ 0 }; // output

		FMOD_VECTOR m_position;
		FMOD_VECTOR m_velocity; // for doppler effects, not really needed?
		FMOD_VECTOR m_forward;
		FMOD_VECTOR m_up;
	};
	template <>
	inline void NewLuaType<AudioListener>(sol::state& luaState, std::string const& name)
	{
		RegisterLuaType<AudioListener>(luaState, name,
			"listener_id", &AudioListener::m_listener_id);
	}
	template<>
	inline void ComponentInspector::operator() < AudioListener > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<AudioListener, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](AudioListener& component, EngineResources& resources)
			{
				(resources); (component);
				ImGui::Separator();
				ImGui::Text("Audio Listener");
				ImGui::Separator();
			});
	}

	template<>
	inline void JSONify<AudioListener>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, AudioListener& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "listener_id", component.m_listener_id);
		// serialize position
	//	RapidJSON::JSONifyToValue(value, document, "position_x", component.m_position.x);
	//	RapidJSON::JSONifyToValue(value, document, "position_y", component.m_position.y);
	//	RapidJSON::JSONifyToValue(value, document, "position_z", component.m_position.z);
	//	// serialize velocity
	//	RapidJSON::JSONifyToValue(value, document, "velocity_x", component.m_velocity.x);
	//	RapidJSON::JSONifyToValue(value, document, "velocity_y", component.m_velocity.y);
	//	RapidJSON::JSONifyToValue(value, document, "velocity_z", component.m_velocity.z);
	//	// serialize forward direction
	//	RapidJSON::JSONifyToValue(value, document, "forward_x", component.m_forward.x);
	//	RapidJSON::JSONifyToValue(value, document, "forward_y", component.m_forward.y);
	//	RapidJSON::JSONifyToValue(value, document, "forward_z", component.m_forward.z);
	//	// serialize up direction
	//	RapidJSON::JSONifyToValue(value, document, "up_x", component.m_up.x);
	//	RapidJSON::JSONifyToValue(value, document, "up_y", component.m_up.y);
	//	RapidJSON::JSONifyToValue(value, document, "up_z", component.m_up.z);
	}

	template<>
	inline void DeJSONify<AudioListener>(EngineResources& engineResources, AudioListener& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.m_listener_id, "listener_id");
		// deserialize position
		//AssignIfExist(jsonObj, component.m_position.x, "position_x");
		//AssignIfExist(jsonObj, component.m_position.y, "position_y");
		//AssignIfExist(jsonObj, component.m_position.z, "position_z");
		//// deserialize velocity
		//AssignIfExist(jsonObj, component.m_velocity.x, "velocity_x");
		//AssignIfExist(jsonObj, component.m_velocity.y, "velocity_y");
		//AssignIfExist(jsonObj, component.m_velocity.z, "velocity_z");
		//// deserialize forward
		//AssignIfExist(jsonObj, component.m_forward.x, "forward_x");
		//AssignIfExist(jsonObj, component.m_forward.y, "forward_y");
		//AssignIfExist(jsonObj, component.m_forward.z, "forward_z");
		//// deserialize up
		//AssignIfExist(jsonObj, component.m_up.x, "up_x");
		//AssignIfExist(jsonObj, component.m_up.y, "up_y");
		//AssignIfExist(jsonObj, component.m_up.z, "up_z");
	}
}