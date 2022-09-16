#pragma once

#include "../godUtility/Utility.h"

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_common.h>

namespace god
{
	struct Sound
	{
		Sound();
		Sound(std::string const& soundPath);

		FMOD::Sound* m_sound_sample;
		std::string m_name;
		std::string m_file_name;

		bool m_played{ false };

		FMOD::Channel* m_channel;
	};

	struct AudioAPI
	{
		AudioAPI();
		~AudioAPI();

		//void CreateFMODSystem();
		//void CreateChannelGroup();
		
		static void LoadSound(const char* filePath, FMOD::Sound** sound);
		static void LoadSound(const char* filePath, Sound& sound);
		static void UnloadSound(FMOD::Sound* sound);

		static void SetLoop(Sound& sound, bool loop);
		static void SetMute(Sound& sound, bool mute);
		static void SetVolume(Sound& sound, float volume);
		static void SetPitch(Sound& sound, float pitch);

		static void Play(FMOD::Sound* sound);
		static void Play(Sound& sound);
		static void PauseAll();
		static void ResumeAll();
		static void StopAll();

	private:
		static FMOD::System* m_FMOD_system;
		static FMOD::ChannelGroup* m_master_channel_group;
		static FMOD::SoundGroup* m_master_sound_group;

	};
}