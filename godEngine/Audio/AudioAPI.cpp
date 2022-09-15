#include "pch.h"
#include "AudioAPI.h"

namespace god
{
	FMOD::System* AudioAPI::m_FMOD_system;
	FMOD::ChannelGroup* AudioAPI::m_master_channel_group;
	FMOD::SoundGroup* AudioAPI::m_master_sound_group;

	AudioAPI::AudioAPI()
	{
		FMOD_RESULT result = FMOD::System_Create(&m_FMOD_system);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		m_FMOD_system->init(64, FMOD_INIT_NORMAL, NULL);

		result = m_FMOD_system->getMasterChannelGroup(&m_master_channel_group);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		result = m_FMOD_system->getMasterSoundGroup(&m_master_sound_group);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		std::cerr << "AudioAPI created\n";
	}

	AudioAPI::~AudioAPI()
	{
		m_FMOD_system->release();
	}


	void AudioAPI::LoadSound(const char* filePath, FMOD::Sound** sound)
	{
		FMOD_RESULT result = m_FMOD_system->createSound(filePath, FMOD_DEFAULT, 0, sound);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));
	}

	void AudioAPI::LoadSound(const char* filePath, Sound& sound)
	{
		FMOD_RESULT result = m_FMOD_system->createSound(filePath, FMOD_DEFAULT, 0, &sound.m_sound_sample);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));
	}

	void AudioAPI::UnloadSound(FMOD::Sound* sound)
	{
		sound->release();
	}


	void AudioAPI::SetLoop(Sound& sound, bool loop)
	{
		sound.m_sound_sample->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		sound.m_sound_sample->setLoopCount(loop ? -1 : 0);
	}

	void AudioAPI::SetMute(Sound& sound, bool mute)
	{
		sound.m_channel->setMute(mute);
	}

	void AudioAPI::SetVolume(Sound& sound, float volume)
	{
		sound.m_channel->setVolume(volume);
	}

	void AudioAPI::SetPitch(Sound& sound, float pitch)
	{
		sound.m_channel->setPitch(pitch);
	}

	void AudioAPI::Play(FMOD::Sound* sound)
	{
		m_FMOD_system->playSound(sound, NULL, false, NULL);
	}

	void AudioAPI::Play(Sound& sound)
	{
		m_FMOD_system->playSound(sound.m_sound_sample, NULL, false, &sound.m_channel);
		sound.m_played = true;
	}

	void AudioAPI::PauseAll()
	{
		m_master_channel_group->setPaused(true);
	}

	void AudioAPI::ResumeAll()
	{
		m_master_channel_group->setPaused(false);
	}

	void AudioAPI::StopAll()
	{
		m_master_channel_group->stop();
		//m_master_sound_group->stop();
	}
}