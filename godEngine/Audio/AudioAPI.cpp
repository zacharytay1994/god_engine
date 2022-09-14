#include "pch.h"
#include "AudioAPI.h"

namespace god
{
	FMOD::System* AudioAPI::m_FMOD_system;
	FMOD::ChannelGroup* AudioAPI::m_master_channel_group;

	AudioAPI::AudioAPI()
	{
		FMOD_RESULT result = FMOD::System_Create(&m_FMOD_system);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		m_FMOD_system->init(64, FMOD_INIT_NORMAL, NULL);

		result = m_FMOD_system->getMasterChannelGroup(&m_master_channel_group);
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

	void AudioAPI::UnloadSound(FMOD::Sound* sound)
	{
		sound->release();
	}

	void AudioAPI::Play(FMOD::Sound* sound)
	{
		m_FMOD_system->playSound(sound, NULL, false, NULL);
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
	}
}