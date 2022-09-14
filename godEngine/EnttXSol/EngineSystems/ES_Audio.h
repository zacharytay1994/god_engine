#pragma once

#include "../EngineComponents/EC_All.h"
#include "../../Audio/AudioAPI.h"


FMOD::System* fmod_system;
FMOD::Channel* channel;
FMOD::ChannelGroup* channel_group;
bool played = false;

float fmod_time = 0.f;

FMOD::Sound* sound;

namespace god
{
	void AudioSystem(AudioSource& audio)
	{
		(audio);

		if (!played)
		{
			AudioAPI::LoadSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\649088__davejf__melody-loop-105-bpm.mp3", &sound);

			AudioAPI::Play(sound);
			played = true;
		}
	}
}