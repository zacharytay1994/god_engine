#pragma once

#include "../godUtility/Utility.h"

#include <fmod.hpp>
#include <fmod_errors.h>

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

		static void Update();

	public:
		static void Create3DReverb(FMOD::Reverb3D** reverb);
		static void SetReverbPreset(FMOD::Reverb3D* reverb);

		static FMOD::ChannelGroup* CreateChannelGroup(const char* name);
		static void SetChannelGroup(int groupID, Sound& sound);

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

		static void PauseAll();
		static void ResumeAll();
		static void StopAndResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets);

	public:
		static void SetListenerAttributes(const FMOD_VECTOR* position, const FMOD_VECTOR* velocity, const FMOD_VECTOR* forward, const FMOD_VECTOR* up);
		static void SetSourceAttributes(Sound& sound, const FMOD_VECTOR* position, const FMOD_VECTOR* velocity);

	public:
		static void GLMVectorToFMODVector(const glm::vec3& input, FMOD_VECTOR& output);
		static std::unordered_map<int, const char*>& GetChannelGroupNames();
		static const char* GetChannelGroupName(int id);

	private:
		static FMOD::System* m_FMOD_system;
		static FMOD::ChannelGroup* m_master_channel_group;
		static FMOD::SoundGroup* m_master_sound_group;

		static std::vector<FMOD::Channel*> m_channels;
		static std::unordered_map<int, FMOD::ChannelGroup*> m_channel_groups;

		static std::unordered_map<int, const char*> m_channel_group_names;
	};
}