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

		FMOD::Channel* m_channel{ nullptr }; // moved to audio source
	};

	struct AudioAPI
	{
		AudioAPI();
		~AudioAPI();

		static void Update();

	public:
		static FMOD::ChannelGroup* CreateChannelGroup(const char* name);
		static void SetChannelGroup(int groupID, FMOD::Channel* channel);

		static void SetChannelGroupMute(int groupID, bool mute);		// for scripting, possibly
		static void SetChannelGroupVolume(int groupID, float volume);	// for scripting, possibly

		static std::unordered_map<int, const char*>& GetChannelGroupNames();
		static const char* GetChannelGroupName(int id);

	public:
		static void LoadSound(const char* filePath, FMOD::Sound** sound);
		static void LoadSound(const char* filePath, Sound& sound);
		static void UnloadSound(Sound& sound);

		static void SetLoop(Sound& sound, bool loop);
		static void SetMute(FMOD::Channel* channel, bool mute);
		static void SetVolume(FMOD::Channel* channel, float volume);
		static void SetPitch(FMOD::Channel* channel, float pitch);

		static void PlaySound(Sound& sound, FMOD::Channel** channel, bool& played); 
		static void PauseSound(Sound& sound, bool paused);
		static void StopSound(FMOD::Channel* channel);

		static void PauseAll();
		static void ResumeAll();
		static void StopAndResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets);

	public:
		static void SetListenerAttributes(const FMOD_VECTOR* position, const FMOD_VECTOR* velocity, const FMOD_VECTOR* forward, const FMOD_VECTOR* up);
		static void SetSourceAttributes(FMOD::Channel* channel, const FMOD_VECTOR* position, const FMOD_VECTOR* velocity);
		static void SetMinMaxDistance(FMOD::Channel* channel , float min, float max);

	public:
		static void GLMVectorToFMODVector(const glm::vec3& input, FMOD_VECTOR& output);

	private:
		static FMOD::System* m_FMOD_system;
		static FMOD::ChannelGroup* m_master_channel_group;
		static FMOD::SoundGroup* m_master_sound_group;

		static std::vector<FMOD::Channel*> m_channels;
		static std::unordered_map<int, FMOD::ChannelGroup*> m_channel_groups;
		static std::unordered_map<int, const char*> m_channel_group_names;
	};
}