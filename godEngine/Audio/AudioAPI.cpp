#include "pch.h"
#include "AudioAPI.h"

namespace god
{
	Sound::Sound() : m_sound_sample{ nullptr }, m_name{}, m_file_name{}, m_channel{ nullptr }
	{
	}

	Sound::Sound(std::string const& soundPath)
	{
		AudioAPI::LoadSound(soundPath.c_str(), *this);
	}

	FMOD::System* AudioAPI::m_FMOD_system;
	FMOD::ChannelGroup* AudioAPI::m_master_channel_group;
	FMOD::SoundGroup* AudioAPI::m_master_sound_group;

	AudioAPI::AudioAPI()
	{
		FMOD_RESULT result = FMOD::System_Create(&m_FMOD_system);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		m_FMOD_system->init(MAX_SOUND_CHANNELS, FMOD_INIT_NORMAL, NULL);

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

		std::string path{ filePath };
		size_t last_slash = path.find_last_of('\\') + 1;
		size_t last_dot = path.find_last_of('.');

		sound.m_file_name = filePath;
		sound.m_name = path.substr(last_slash, last_dot - last_slash);
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

	void AudioAPI::PlaySound(Sound& sound)
	{
		m_FMOD_system->playSound(sound.m_sound_sample, NULL, false, &sound.m_channel);
		sound.m_played = true;
	}

	void AudioAPI::PauseSound(Sound& sound, bool paused)
	{
		sound.m_channel->setPaused(paused);
	}

	void AudioAPI::StopSound(Sound& sound)
	{
		sound.m_channel->stop();
	}

	void AudioAPI::ResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets)
	{
		for (auto& asset : assets)
		{
			Sound& sound = const_cast<Sound&>(std::get<1>(asset));
			sound.m_played = false;
		}
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