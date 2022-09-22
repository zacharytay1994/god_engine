#pragma once

#include "../godUtility/Utility.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#define MAX_SOUND_CHANNELS 64

namespace god
{
	struct Sound
	{
		Sound();
		Sound(std::string const& soundPath);

		FMOD::Sound* m_sound_sample;
		std::string m_name;
		std::string m_file_name;

		bool m_played{ false }; // moved to audio source

		FMOD::Channel* m_channel{ nullptr };
	};

	struct AudioAPI
	{
		AudioAPI();
		~AudioAPI();

	public:
		//static void Create3DReverb(FMOD::Reverb3D** reverb);
		//static void SetReverbPreset(FMOD::Reverb3D* reverb);

		//void CreateChannelGroup();

	public:
		static void LoadSound(const char* filePath, FMOD::Sound** sound);
		static void LoadSound(const char* filePath, Sound& sound);
		static void UnloadSound(FMOD::Sound* sound);
		static void UnloadSound(Sound& sound);

		static void SetLoop(Sound& sound, bool loop);
		static void SetMute(Sound& sound, bool mute);
		static void SetVolume(Sound& sound, float volume);
		static void SetPitch(Sound& sound, float pitch);

		static void PlaySound(Sound& sound);
		static void PlaySound(Sound& sound, bool& played); 
		static void PauseSound(Sound& sound, bool paused);
		static void StopSound(Sound& sound);

		static void ResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets);
		static void PauseAll();
		static void ResumeAll();
		static void StopAll();

	private:
		static FMOD::System* m_FMOD_system;
		static FMOD::ChannelGroup* m_master_channel_group;
		static FMOD::SoundGroup* m_master_sound_group;

		static std::vector<FMOD::Channel*> m_channels;
	};
}