#pragma once

#include "../EngineComponents/EC_All.h"

#include <fmod_errors.h>
FMOD::System* fmod_system;
FMOD::Channel* channel;
FMOD::ChannelGroup* channel_group;
bool played = false;

float fmod_time = 0.f;

namespace god
{
	void AudioSystem(AudioSource& audio)
	{
		(audio);
		
		if (!fmod_system)
		{
			if (FMOD::System_Create(&fmod_system) != FMOD_OK)
				std::cerr << FMOD_ErrorString(FMOD::System_Create(&fmod_system, NULL)) << std::endl;

			fmod_system->init(36, FMOD_INIT_NORMAL, NULL);
		}

		fmod_system->getMasterChannelGroup(&channel_group);

		FMOD::Sound* sound;
		FMOD_RESULT result = fmod_system->createSound("C:\\Users\\Tingl\\Desktop\\CSD3400\\god_engine\\1_Game\\Assets\\GameAssets\\Sounds\\649088__davejf__melody-loop-105-bpm.mp3", FMOD_DEFAULT, 0, &sound);
		if (result != FMOD_OK)
			std::cerr << FMOD_ErrorString(result) << std::endl;
		//else
		//	std::cerr << "sound loaded successfully" << std::endl;

		if (!played)
		{
			fmod_system->playSound(sound, nullptr, false, nullptr);
			played = true;
		}

		fmod_time += 0.1f;

		if (fmod_time > 3.f && fmod_time < 5.f)
			channel_group->stop();
		//channel_group->setPaused(true);
		else if (fmod_time > 5.f)
		{
			fmod_time = 0.f;
			played = false;
		}
			//channel_group->setPaused(false);
	}
}