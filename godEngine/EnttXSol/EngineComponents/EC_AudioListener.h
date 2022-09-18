#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct AudioListener
	{
		// audio clip reference

		int m_listener_id{ -1 }; // output

		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_forward;
		glm::vec3 m_up;
	};
	template <>
	inline void NewLuaType<AudioListener>(sol::state& luaState, std::string const& name)
	{
		//RegisterLuaType<AudioListener>(luaState, name,
		//	"source_id", &AudioSource::m_source_id,
		//	"listener_id", &AudioSource::m_listener_id,
		//	"mute", &AudioSource::m_mute,
		//	"loop", &AudioSource::m_loop,
		//	"volume", &AudioSource::m_volume,
		//	"pitch", &AudioSource::m_pitch,
		//	"min_distance", &AudioSource::m_min_distance,
		//	"max_distance", &AudioSource::m_max_distance);
	}
	template<>
	inline void ComponentInspector::operator() < AudioListener > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<AudioListener, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](AudioListener& component, EngineResources& resources)
			{
				(resources);
				ImGui::Separator();
				ImGui::Text("Audio Source");
				ImGui::Separator();

				//ImGui::ListBox("Output", &component.m_listener_id, nullptr, 0, 5);

				//ImGui::Checkbox("Mute", &component.m_mute);
				//ImGui::Checkbox("Loop", &component.m_loop);

				//ImGui::SliderFloat("Volume", &component.m_volume, 0.f, 1.f, "%.1f", 1.f);
				//ImGui::SliderFloat("Pitch", &component.m_pitch, 0.f, 1.5f, "%.1f", 1.f);
				//ImGui::InputFloat("Min Distance", &component.m_min_distance);
				//ImGui::InputFloat("Max Distance", &component.m_max_distance);
			});
	}

	template<>
	inline void JSONify<AudioListener>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, AudioListener& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "listener_id", component.m_listener_id);
		// serialize position
		RapidJSON::JSONifyToValue(value, document, "position_x", component.m_position.x);
		RapidJSON::JSONifyToValue(value, document, "position_y", component.m_position.y);
		RapidJSON::JSONifyToValue(value, document, "position_z", component.m_position.z);
		// serialize velocity
		RapidJSON::JSONifyToValue(value, document, "velocity_x", component.m_velocity.x);
		RapidJSON::JSONifyToValue(value, document, "velocity_y", component.m_velocity.y);
		RapidJSON::JSONifyToValue(value, document, "velocity_z", component.m_velocity.z);
		// serialize forward direction
		RapidJSON::JSONifyToValue(value, document, "forward_x", component.m_forward.x);
		RapidJSON::JSONifyToValue(value, document, "forward_y", component.m_forward.y);
		RapidJSON::JSONifyToValue(value, document, "forward_z", component.m_forward.z);
		// serialize up direction
		RapidJSON::JSONifyToValue(value, document, "up_x", component.m_up.x);
		RapidJSON::JSONifyToValue(value, document, "up_y", component.m_up.y);
		RapidJSON::JSONifyToValue(value, document, "up_z", component.m_up.z);
	}

	template<>
	inline void DeJSONify<AudioListener>(EngineResources& engineResources, AudioListener& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		//AssignIfExist(jsonObj, component.m_source_id, "source_id");
		//AssignIfExist(jsonObj, component.m_listener_id, "listener_id");
		//AssignIfExist(jsonObj, component.m_mute, "mute");
		//AssignIfExist(jsonObj, component.m_loop, "loop");
		//AssignIfExist(jsonObj, component.m_volume, "volume");
		//AssignIfExist(jsonObj, component.m_pitch, "pitch");
		//AssignIfExist(jsonObj, component.m_min_distance, "min_distance");
		//AssignIfExist(jsonObj, component.m_max_distance, "max_distance");
	}
}