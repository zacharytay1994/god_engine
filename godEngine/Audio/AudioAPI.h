#pragma once

#include "../godUtility/Utility.h"

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_common.h>

namespace god
{
	struct AudioAPI
	{
		AudioAPI();
		~AudioAPI();

		//void CreateFMODSystem();
		//void CreateChannelGroup();
		//
		static void LoadSound(const char* filePath, FMOD::Sound** sound);
		static void UnloadSound(FMOD::Sound* sound);

		//void Loop();
		//void Mute();
		//void AdjustVolume();

		static void Play(FMOD::Sound* sound);
		static void PauseAll();
		static void ResumeAll();
		static void StopAll();

	private:
		static FMOD::System* m_FMOD_system;
		static FMOD::ChannelGroup* m_master_channel_group;
	};
}