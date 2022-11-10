#pragma once

#include "EngineComponents.h"

namespace god
{
	/* ENGINE COMPONENTS */
	struct AudioSource
	{
		FMOD::Channel* m_channel{ nullptr };
		const char* m_channel_group_name{ "" };
		int m_channel_group_id{ 0 }; // to seperate sounds into different categories

		int m_sound_id{ -1 };
		bool m_played{ true };
		bool m_finish_playing{ false };

		bool m_mute{ false };
		bool m_loop{ false };

		bool m_play_on_awake{ true };
		bool m_awake_played{ false };

		float m_volume{ 1.f };
		float m_pitch{ 1.f };
		float m_min_distance{ 1.f };
		float m_max_distance{ 10.f };
		
		FMOD_VECTOR m_position;
		FMOD_VECTOR m_velocity;
 	};
	template <>
	inline void NewLuaType<AudioSource>(sol::state& luaState, std::string const& name)
	{
		RegisterLuaType<AudioSource>(luaState, name,
			"sound_id", &AudioSource::m_sound_id,
			"finish_playing", &AudioSource::m_played,
			"mute", &AudioSource::m_mute,
			"loop", &AudioSource::m_loop,
			"play_on_awake", &AudioSource::m_play_on_awake,
			"volume", &AudioSource::m_volume,
			"pitch", &AudioSource::m_pitch,
			"min_distance", &AudioSource::m_min_distance,
			"max_distance", &AudioSource::m_max_distance
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

				// select sound
				if (ImGui::BeginPopup("Sound Select"))
				{
					for (auto const& asset : sounds.get().GetIDs())
					{
						if (ImGui::Selectable(asset.first.c_str()))
						{
							if (component.m_sound_id != -1)
							{
								if (component.m_channel)
									AudioAPI::StopSound(component.m_channel);
							}

							component.m_sound_id = asset.second;
							component.m_played = false;

							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndPopup();
				}

				ImGui::Text(" Audio Clip : ");
				if (ImGui::Button(sounds.get().GetName(component.m_sound_id).c_str(), { ImGui::GetWindowWidth(),0 }))
				{
					ImGui::OpenPopup("Sound Select");
				}

				// select sound group
				if (ImGui::BeginPopup("Sound Group Select"))
				{
					for (auto const& name : AudioAPI::GetChannelGroupNames())
					{
						if (ImGui::Selectable(std::get<1>(name)))
						{
							component.m_channel_group_id = std::get<0>(name);
							if (component.m_channel)
								AudioAPI::SetChannelGroup(component.m_channel_group_id, component.m_channel);

							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndPopup();
				}

				ImGui::Text(" Sound Group: ");
				if (ImGui::Button(AudioAPI::GetChannelGroupName(component.m_channel_group_id), { ImGui::GetWindowWidth(),0 }))
				{
					ImGui::OpenPopup("Sound Group Select");
				}

				// manually play sound
				if (ImGui::Button("Play Sound"))
				{
					if (component.m_sound_id != -1)
					{
						if (component.m_channel)
							AudioAPI::StopSound(component.m_channel);
					}

					component.m_played = false;
				}

				// modify other properties
				ImGui::Checkbox("Mute", &component.m_mute);
				ImGui::Checkbox("Loop", &component.m_loop);
				ImGui::Checkbox("Play On Awake", &component.m_play_on_awake);

				ImGui::SliderFloat("Volume", &component.m_volume, 0.f, 2.f, "%.01f", 1.f);
				ImGui::SliderFloat("Pitch", &component.m_pitch, 0.f, 1.5f, "%.01f", 1.f);
				ImGui::InputFloat("Min Distance", &component.m_min_distance);
				ImGui::InputFloat("Max Distance", &component.m_max_distance);
			});
	}

	template<>
	inline void JSONify<AudioSource>(EngineResources& engineResources, rapidjson::Document& document, rapidjson::Value& value, AudioSource& component)
	{
		(engineResources);
		// serialize
		RapidJSON::JSONifyToValue(value, document, "sound_id", component.m_sound_id);
		RapidJSON::JSONifyToValue(value, document, "channel_group_id", component.m_channel_group_id);
		RapidJSON::JSONifyToValue(value, document, "mute", component.m_mute);
		RapidJSON::JSONifyToValue(value, document, "loop", component.m_loop);
		RapidJSON::JSONifyToValue(value, document, "play_on_awake", component.m_play_on_awake);
		RapidJSON::JSONifyToValue(value, document, "volume", component.m_volume);
		RapidJSON::JSONifyToValue(value, document, "pitch", component.m_pitch);
		RapidJSON::JSONifyToValue(value, document, "min_distance", component.m_min_distance);
		RapidJSON::JSONifyToValue(value, document, "max_distance", component.m_max_distance);
	}

	template<>
	inline void DeJSONify<AudioSource>(EngineResources& engineResources, AudioSource& component, rapidjson::Value& jsonObj)
	{
		(engineResources);
		// deserialize
		AssignIfExist(jsonObj, component.m_sound_id, "sound_id");
		AssignIfExist(jsonObj, component.m_channel_group_id, "channel_group_id");
		AssignIfExist(jsonObj, component.m_mute, "mute");
		AssignIfExist(jsonObj, component.m_loop, "loop");
		AssignIfExist(jsonObj, component.m_play_on_awake, "play_on_awake");
		AssignIfExist(jsonObj, component.m_volume, "volume");
		AssignIfExist(jsonObj, component.m_pitch, "pitch");
		AssignIfExist(jsonObj, component.m_min_distance, "min_distance");
		AssignIfExist(jsonObj, component.m_max_distance, "max_distance");
	}
}