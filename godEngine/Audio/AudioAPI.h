#pragma once

#include "../godUtility/Utility.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#define UINTLL unsigned long long 

namespace god
{
	struct Sound
	{
		Sound();
		Sound(std::string const& soundPath);

		FMOD::Sound* m_sound_sample{ nullptr };
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
		static void ErrorCheck(FMOD_RESULT result)
		{
			if (result != FMOD_OK)
				assert(FMOD_ErrorString(result));
		}

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

		static bool GetLoop(Sound& sound);
		static bool GetMute(FMOD::Channel* channel);
		static float GetVolume(FMOD::Channel* channel);
		static float GetPitch(FMOD::Channel* channel);

		static void PlaySound(Sound& sound); 
		static void PlaySound(Sound& sound, FMOD::Channel** channel, bool& played); 
		static void PauseSound(FMOD::Channel* channel , bool paused);
		static void PauseSound(Sound& sound, bool paused);
		static void StopSound(FMOD::Channel* channel);

		static FMOD_RESULT CheckSoundPlayback(FMOD::Channel* channel, bool* isPlaying);

		static void PlaySoundFromTime(FMOD::Channel* channel, float startPoint);
		static void PlaySoundUntilTime(FMOD::Channel* channel, float endPoint);
		static void PlaySoundBetweenTimes(FMOD::Channel* channel, float startPoint, float endPoint);

		static void PauseAll();
		static void ResumeAll();
		static void StopAndResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets);

	public:
		static void ToggleDSPEffects(bool toggle);
		static void AddEcho(FMOD::Channel* channel);
		static void AddFadeIn(FMOD::Channel* channel, float fadeInTime, bool& fade);
		static void AddFadeOut(FMOD::Channel* channel, float fadeOutTime, bool& fade);

		static void RemoveFadeIn(FMOD::Channel* channel, float fadeInTime);
		static void RemoveFadeOut(FMOD::Channel* channel, float fadeOutPoint);

		static void GetFadePoints(FMOD::Channel* channel, unsigned int* points);
		static int GetSampleRate();
		static UINTLL GetDSPClock(FMOD::Channel* channel);

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

		static std::list<FMOD::Channel*> m_channels;
		static std::unordered_map<int, FMOD::ChannelGroup*> m_channel_groups;
		static std::unordered_map<int, const char*> m_channel_group_names;

		static std::vector<Sound> m_extra_sounds;

		// DSP Effects
		static int m_sample_rate;

		static FMOD::DSP* dsp_echo;

		static std::unordered_map<int, FMOD::DSP*> m_dsp_effects;
		static std::unordered_map<int, const char*> m_dsp_effects_names;

	};
}