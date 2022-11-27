#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

namespace god
{
	void AudioSourceSystem(EnttXSol& entt, EngineResources& engineResources, std::tuple<EntityData&, AudioSource, Transform&> components)
	{
		(entt); (engineResources);

		AudioSource& audio_source = std::get<1>(components);
		Transform& transform = std::get<2>(components);

		if (audio_source.m_sound_id != -1)
		{
			if (audio_source.m_channel != nullptr)
			{
				// check for playback state
				AudioAPI::CheckSoundPlayback(audio_source.m_channel, &audio_source.m_finish_playing);

				//// Get current playtime
				//AudioAPI::GetCurrentPlayTime(audio_source.m_channel, &audio_source.m_current_playtime);
				////std::cout << audio_source.m_current_playtime << "\n";

				// Set Audio Source attributes
				AudioAPI::SetLoop(audio_source.m_channel, audio_source.m_loop);
				AudioAPI::SetMute(audio_source.m_channel, audio_source.m_mute);
				AudioAPI::SetVolume(audio_source.m_channel, audio_source.m_volume);
				AudioAPI::SetPitch(audio_source.m_channel, audio_source.m_pitch);

				// set min and max distance for attenuation
				AudioAPI::SetMinMaxDistance(audio_source.m_channel, audio_source.m_min_distance, audio_source.m_max_distance);

				// 3d sound
				AudioAPI::GLMVectorToFMODVector(transform.m_position, audio_source.m_position);
				AudioAPI::GLMVectorToFMODVector(glm::vec3(0.f), audio_source.m_velocity);

				AudioAPI::SetSourceAttributes(audio_source.m_channel, &audio_source.m_position, &audio_source.m_velocity);

			}
		}
	}
}