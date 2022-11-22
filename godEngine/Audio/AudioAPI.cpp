#include "../pch.h"
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

	std::list<FMOD::Channel*> AudioAPI::m_channels;
	std::unordered_map<int, FMOD::ChannelGroup*> AudioAPI::m_channel_groups;

	std::unordered_map<int, const char*> AudioAPI::m_channel_group_names =
	{
		{ 1, "Default" }, { 2, "Music" }, { 3, "SFX" }
	};

	std::vector<Sound> AudioAPI::m_extra_sounds;

	// DSP Effects
	FMOD::DSP* AudioAPI::dsp_echo;

	AudioAPI::AudioAPI()
	{
		FMOD_RESULT result = FMOD::System_Create(&m_FMOD_system);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		m_FMOD_system->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, NULL);

		result = m_FMOD_system->getMasterChannelGroup(&m_master_channel_group);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		result = m_FMOD_system->getMasterSoundGroup(&m_master_sound_group);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		for (const auto& name : m_channel_group_names)
		{
			if (name.first == 0)
				continue;

			m_channel_groups.insert({ name.first, CreateChannelGroup(name.second) });
		}

		result = m_FMOD_system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsp_echo);

		std::cerr << "AudioAPI created\n";
	}

	AudioAPI::~AudioAPI()
	{
		m_FMOD_system->release();

		for (auto& sound : m_extra_sounds)
			UnloadSound(sound);
	}

	void AudioAPI::Update()
	{
		m_FMOD_system->update();
		
		// check for inactive channels and remove them
		bool isPlaying = true;
		FMOD::Channel* channel_ptr = nullptr;

		for (auto& channel : m_channels)
		{
			channel->isPlaying(&isPlaying);
			if (!isPlaying)
			{
				channel_ptr = channel;
				break;
			}
		}
		m_channels.remove(channel_ptr);
	}

	FMOD::ChannelGroup* AudioAPI::CreateChannelGroup(const char* name)
	{
		FMOD::ChannelGroup* channel_group = nullptr;

		FMOD_RESULT result = m_FMOD_system->createChannelGroup(name, &channel_group);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));

		return channel_group;
	}

	void AudioAPI::SetChannelGroup(int groupID, FMOD::Channel* channel)
	{
		channel->setChannelGroup(m_channel_groups.at(groupID));
	}

	void AudioAPI::SetChannelGroupMute(int groupID, bool mute)
	{
		FMOD::ChannelGroup* channel_group = m_channel_groups.at(groupID);
		channel_group->setMute(mute);

		channel_group = nullptr;
	}

	void AudioAPI::SetChannelGroupVolume(int groupID, float volume)
	{
		FMOD::ChannelGroup* channel_group = m_channel_groups.at(groupID);
		channel_group->setVolume(volume);

		channel_group = nullptr;
	}


	void AudioAPI::LoadSound(const char* filePath, FMOD::Sound** sound)
	{
		FMOD_RESULT result = m_FMOD_system->createSound(filePath, FMOD_DEFAULT, 0, sound);
		if (result != FMOD_OK)
			assert(FMOD_ErrorString(result));
	}

	void AudioAPI::LoadSound(const char* filePath, Sound& sound)
	{
		// change the mode when creating sound
		FMOD_MODE mode = FMOD_3D;

		if (!sound.m_sound_sample)
		{
			FMOD_RESULT result = m_FMOD_system->createSound(filePath, mode, 0, &sound.m_sound_sample);
			if (result != FMOD_OK)
				assert(FMOD_ErrorString(result));
		}

		std::string path{ filePath };
		size_t last_slash = path.find_last_of('\\') + 1;
		size_t last_dot = path.find_last_of('.');

		sound.m_file_name = filePath;
		sound.m_name = path.substr(last_slash, last_dot - last_slash);
	}

	void AudioAPI::UnloadSound(Sound& sound)
	{
		sound.m_sound_sample->release();
	}


	void AudioAPI::SetLoop(Sound& sound, bool loop)
	{
		sound.m_sound_sample->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		sound.m_sound_sample->setLoopCount(loop ? -1 : 0);
	}

	void AudioAPI::SetMute(FMOD::Channel* channel, bool mute)
	{
		channel->setMute(mute);
	}

	void AudioAPI::SetVolume(FMOD::Channel* channel, float volume)
	{
		channel->setVolume(volume);
	}

	void AudioAPI::SetPitch(FMOD::Channel* channel, float pitch)
	{
		channel->setPitch(pitch);
	}

	void AudioAPI::PlaySound(Sound& sound)
	{
		m_FMOD_system->playSound(sound.m_sound_sample, NULL, false, &sound.m_channel);
		sound.m_played = true;

		m_channels.push_back(sound.m_channel);
	}

	void AudioAPI::PlaySound(Sound& sound, FMOD::Channel** channel, bool& played)
	{
		m_FMOD_system->playSound(sound.m_sound_sample, NULL, false, channel);
		played = true;

		m_channels.push_back(*channel);
	}

	void AudioAPI::PauseSound(Sound& sound, bool paused)
	{
		sound.m_channel->setPaused(paused);
	}

	void AudioAPI::PauseSound(FMOD::Channel* channel, bool paused)
	{
		channel->setPaused(paused);
	}

	void AudioAPI::StopSound(FMOD::Channel* channel)
	{
		channel->stop();
	}

	FMOD_RESULT AudioAPI::CheckSoundPlayback(FMOD::Channel* channel, bool* isPlaying)
	{
		channel->isPlaying(isPlaying);

		return FMOD_OK;
	}

	void AudioAPI::StopAndResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets)
	{
		m_master_channel_group->stop();

		for (auto& asset : assets)
		{
			Sound& sound = const_cast<Sound&>(std::get<1>(asset));
			sound.m_played = false;
		}

		m_channels.clear();
	}

	void AudioAPI::ToggleDSPEffects(bool toggle)
	{
		dsp_echo->setBypass(!toggle);
	}

	void AudioAPI::AddEchoEffect(FMOD::Channel* channel)
	{
		channel->addDSP(0, dsp_echo);
	}


	void AudioAPI::PauseAll()
	{
		m_master_channel_group->setPaused(true);
	}

	void AudioAPI::ResumeAll()
	{
		m_master_channel_group->setPaused(false);
	}


	void AudioAPI::SetListenerAttributes(const FMOD_VECTOR* position, const FMOD_VECTOR* velocity, const FMOD_VECTOR* forward, const FMOD_VECTOR* up)
	{
		m_FMOD_system->set3DListenerAttributes(0, position, velocity, forward, up);
	}

	void AudioAPI::SetSourceAttributes(FMOD::Channel* channel, const FMOD_VECTOR* position, const FMOD_VECTOR* velocity)
	{
		channel->set3DAttributes(position, velocity);
	}

	void AudioAPI::SetMinMaxDistance(FMOD::Channel* channel, float min, float max)
	{
		channel->set3DMinMaxDistance(min, max);
	}


	void AudioAPI::GLMVectorToFMODVector(const glm::vec3& input, FMOD_VECTOR& output)
	{
		output.x = input.x;
		output.y = input.y;
		output.z = input.z;
	}

	std::unordered_map<int, const char*>& AudioAPI::GetChannelGroupNames()
	{
		return m_channel_group_names;
	}

	const char* AudioAPI::GetChannelGroupName(int id)
	{
		for (auto const& name : m_channel_group_names)
		{
			if (name.first == id)
			{
				return name.second;
			}
		}

		return "None";
	}
}