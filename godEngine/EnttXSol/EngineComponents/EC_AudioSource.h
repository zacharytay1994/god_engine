#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct AudioSource
	{
		// audio clip reference
		Sound m_sound;
		std::string m_file;

		// geometry for occlusion

		//int m_channel_group;

		bool m_3d_sound{ false };

		int m_sound_id{ -1 };
		int m_source_id{ -1 };
		int m_listener_id{ -1 }; // output

		bool m_mute{ false };
		bool m_loop{ false };
		bool m_play_on_awake{ true };

		float m_volume{ 1.f }; // how loud from the audio listener (for now just adjust volume of sound)
		float m_pitch{ 1.f };
		//float m_min_distance;
		//float m_max_distance;
	};
	template <>
	inline void NewLuaType<AudioSource>(sol::state& luaState, std::string const& name)
	{
		RegisterLuaType<AudioSource>(luaState, name,
			"file", &AudioSource::m_file,
			"source_id", &AudioSource::m_source_id,
			//"listener_id", &AudioSource::m_listener_id,
			"mute", &AudioSource::m_mute,
			"loop", &AudioSource::m_loop,
			"play_on_awake", &AudioSource::m_play_on_awake,
			"volume", &AudioSource::m_volume,
			"pitch", &AudioSource::m_pitch
			//"min_distance", &AudioSource::m_min_distance,
			//"max_distance", &AudioSource::m_max_distance
			);
	}
	template<>
	inline void ComponentInspector::operator() < AudioSource > (entt::entity entity, entt::registry& registry, int& imguiUniqueID, EngineResources& editorResources)
	{
		RegisterInspector<AudioSource, EngineResources>(entity, registry, imguiUniqueID, editorResources,
			[](AudioSource& component, EngineResources& resources)
			{
				auto& sounds = resources.Get<SoundManager>();

				ImGui::Separator();
				ImGui::Text("Audio Source");
				ImGui::Separator();

				if (ImGui::BeginPopup("Sound Select"))
				{
					for (auto const& asset : sounds.get().GetIDs())
					{
						if (ImGui::Selectable(asset.first.c_str()))
						{
							component.m_sound_id = asset.second;
							ImGui::CloseCurrentPopup();

							AudioAPI::StopAll();
						}
					}
					ImGui::EndPopup();
				}

				ImGui::Text(" Audio Clip : ");
				if (ImGui::Button(sounds.get().GetName(component.m_sound_id).c_str(), { ImGui::GetWindowWidth(),0 }))
				{
					ImGui::OpenPopup("Sound Select");
				}

				//ImGui::ListBox("Output", &component.m_listener_id, nullptr, 0, 5);

				ImGui::Checkbox("Mute", &component.m_mute);
				ImGui::Checkbox("Loop", &component.m_loop);
				ImGui::Checkbox("Play On Awake", &component.m_play_on_awake);

				ImGui::SliderFloat("Volume", &component.m_volume, 0.f, 1.f, "%.1f", 1.f);
				ImGui::SliderFloat("Pitch", &component.m_pitch, 0.f, 1.5f, "%.1f", 1.f);
				//ImGui::InputFloat("Min Distance", &component.m_min_distance);
				//ImGui::InputFloat("Max Distance", &component.m_max_distance);
			});
	}

	template<>
	inline void JSONify<AudioSource>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, AudioSource& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "file", component.m_file);
		RapidJSON::JSONifyToValue(value, document, "source_id", component.m_source_id);
		//RapidJSON::JSONifyToValue(value, document, "listener_id", component.m_listener_id);
		RapidJSON::JSONifyToValue(value, document, "mute", component.m_mute);
		RapidJSON::JSONifyToValue(value, document, "loop", component.m_loop);
		RapidJSON::JSONifyToValue(value, document, "play_on_awake", component.m_play_on_awake);
		RapidJSON::JSONifyToValue(value, document, "volume", component.m_volume);
		RapidJSON::JSONifyToValue(value, document, "pitch", component.m_pitch);
		//RapidJSON::JSONifyToValue(value, document, "min_distance", component.m_min_distance);
		//RapidJSON::JSONifyToValue(value, document, "max_distance", component.m_max_distance);
	}

	template<>
	inline void DeJSONify<AudioSource>(EngineResources& engineResources, AudioSource& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.m_file, "file");
		AssignIfExist(jsonObj, component.m_source_id, "source_id");
		//AssignIfExist(jsonObj, component.m_listener_id, "listener_id");
		AssignIfExist(jsonObj, component.m_mute, "mute");
		AssignIfExist(jsonObj, component.m_loop, "loop");
		AssignIfExist(jsonObj, component.m_play_on_awake, "play_on_awake");
		AssignIfExist(jsonObj, component.m_volume, "volume");
		AssignIfExist(jsonObj, component.m_pitch, "pitch");
		//AssignIfExist(jsonObj, component.m_min_distance, "min_distance");
		//AssignIfExist(jsonObj, component.m_max_distance, "max_distance");
	}
}