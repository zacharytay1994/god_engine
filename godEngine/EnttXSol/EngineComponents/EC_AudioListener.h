#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct AudioListener
	{
		int m_listener_id{ 0 }; // output

		FMOD_VECTOR m_position;
		FMOD_VECTOR m_forward;
		FMOD_VECTOR m_up;

		bool dsp_effects_on = true;

		bool operator==( AudioListener const& rhs )
		{
			return m_listener_id == rhs.m_listener_id;
		}
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

				ImGui::Checkbox("Enable Effects", &component.dsp_effects_on);
			});
	}

	template<>
	inline void JSONify<AudioListener>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, AudioListener& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "listener_id", component.m_listener_id);
	}

	template<>
	inline void DeJSONify<AudioListener>(EngineResources& engineResources, AudioListener& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.m_listener_id, "listener_id");
	}
}