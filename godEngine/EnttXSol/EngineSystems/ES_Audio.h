#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"

bool playing = false;

namespace god
{
	void AudioSystem ( EnttXSol& entt , EngineResources& engineResources , std::tuple<EntityData& , AudioListener&> components )
	{
		( entt ); ( components );

		// NOTE: ONLY ONE AUDIO LISTENER ALLOWED

		// engine resources access
		SoundManager& sound_manager = engineResources.Get<SoundManager> ().get ();

		// access entities with specific component
		for ( auto&& [entity , active , audio_source] : entt.GetView<AudioSource> ().each () )
		{
			if ( audio_source.m_sound_id != -1 )
			{
				if ( !audio_source.m_3d_sound )
				{
					auto& resource = sound_manager.Get ( audio_source.m_sound_id );
					Sound& sound = std::get<1> ( resource );

					if ( audio_source.m_reverb == nullptr )
					{
						AudioAPI::Create3DReverb ( &audio_source.m_reverb );
					}

					AudioAPI::SetLoop ( sound , audio_source.m_loop );

					if ( audio_source.m_play_on_awake && !audio_source.m_played ) // modify a bit to check for m_play_on_awake
					{
						AudioAPI::PlaySound ( sound , audio_source.m_played );
					}

					AudioAPI::SetMute ( sound , audio_source.m_mute );
					AudioAPI::SetVolume ( sound , audio_source.m_volume );
					AudioAPI::SetPitch ( sound , audio_source.m_pitch );
				}
				else
				{
					// 3d sound
					// get transform component, fmod is left handed system (need to set listener attributes)
				}
			}
		}
	}
}