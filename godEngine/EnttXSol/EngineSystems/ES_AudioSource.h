#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

namespace god
{
	void AudioSourceSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioSource, Transform&> components)
	{
		(entt);

		AudioSource& audio_source = std::get<1>(components);
		Transform& transform = std::get<2>(components);

		// engine resources access
		SoundManager& sound_manager = engineResources.Get<SoundManager>().get();

		if (audio_source.m_sound_id != -1)
		{
			auto& resource = sound_manager.Get(audio_source.m_sound_id);
			Sound& sound = std::get<1>(resource);

			if (sound.m_channel != nullptr)
			{
				// 3d sound
				AudioAPI::GLMVectorToFMODVector(transform.m_position, audio_source.m_position);
				AudioAPI::GLMVectorToFMODVector(glm::vec3(0.f), audio_source.m_velocity);

				AudioAPI::SetSourceAttributes(sound, &audio_source.m_position, &audio_source.m_velocity);
			}
		}
	}
}