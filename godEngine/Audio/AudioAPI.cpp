#include "../pch.h"
#include "AudioAPI.h"

namespace god
{
	Sound::Sound() : m_sound_sample{ nullptr }, m_name{}, m_file_name{}, m_channel{ nullptr }, m_length{ 0 }
	{
	}

	Sound::Sound(std::string const& soundPath)
	{
		AudioAPI::LoadSound(soundPath.c_str(), *this);
	}


	// AudioAPI members
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
	int AudioAPI::m_sample_rate;

	FMOD::DSP* AudioAPI::dsp_echo;
	std::unordered_map <int, FMOD::DSP* > m_dsp_effects;

	std::unordered_map<int, const char*> AudioAPI::m_dsp_effects_names =
	{
		{ 1, "Echo" }, { 2, "Fade" }
	};

	Sound AudioAPI::BGM;

	AudioAPI::AudioAPI()
	{
		ErrorCheck(FMOD::System_Create(&m_FMOD_system));
		m_FMOD_system->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, NULL);

		ErrorCheck(m_FMOD_system->getMasterChannelGroup(&m_master_channel_group));
		ErrorCheck(m_FMOD_system->getMasterSoundGroup(&m_master_sound_group));

		for (const auto& name : m_channel_group_names)
		{
			if (name.first == 0)
				continue;

			m_channel_groups.insert({ name.first, CreateChannelGroup(name.second) });
		}

		// DSP Effects
		{
			ErrorCheck(m_FMOD_system->getSoftwareFormat(&m_sample_rate, nullptr, nullptr));

			ErrorCheck(m_FMOD_system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &dsp_echo));
		}

		LoadSound("Assets/GameAssets/Sounds/MainMenu_Melody_v1_20221122.mp3", BGM);
		SetLoop(BGM, true);

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
		ErrorCheck(m_FMOD_system->createChannelGroup(name, &channel_group));

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
		ErrorCheck(m_FMOD_system->createSound(filePath, FMOD_DEFAULT, 0, sound));
	}

	void AudioAPI::LoadSound(const char* filePath, Sound& sound)
	{
		// change the mode when creating sound
		FMOD_MODE mode = FMOD_3D;

		if (!sound.m_sound_sample)
		{
			ErrorCheck(m_FMOD_system->createSound(filePath, mode, 0, &sound.m_sound_sample));
		}
		unsigned int sound_length;
		sound.m_sound_sample->getLength(&sound_length, FMOD_TIMEUNIT_MS);
		sound.m_length = static_cast<double>(sound_length) * 0.001; // convert milliseconds to seconds

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
		ErrorCheck(sound.m_sound_sample->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
		ErrorCheck(sound.m_sound_sample->setLoopCount(loop ? -1 : 0));
	}

	void AudioAPI::SetLoop(FMOD::Channel* channel, bool loop)
	{
		ErrorCheck(channel->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF));
		ErrorCheck(channel->setLoopCount(loop ? -1 : 0));
	}

	void AudioAPI::SetMute(FMOD::Channel* channel, bool mute)
	{
		ErrorCheck(channel->setMute(mute));
	}

	void AudioAPI::SetVolume(FMOD::Channel* channel, float volume)
	{
		ErrorCheck(channel->setVolume(volume));
	}

	void AudioAPI::SetPitch(FMOD::Channel* channel, float pitch)
	{
		ErrorCheck(channel->setPitch(pitch));
	}

	bool AudioAPI::GetLoop(Sound& sound)
	{
		int loop_count;
		ErrorCheck(sound.m_sound_sample->getLoopCount(&loop_count));

		return loop_count == -1 ? true : false;
	}

	bool AudioAPI::GetMute(FMOD::Channel* channel)
	{
		bool mute;
		ErrorCheck(channel->getMute(&mute));

		return mute;
	}

	float AudioAPI::GetVolume(FMOD::Channel* channel)
	{
		float volume;
		ErrorCheck(channel->getVolume(&volume));

		return volume;
	}

	float AudioAPI::GetPitch(FMOD::Channel* channel)
	{
		float pitch;
		ErrorCheck(channel->getPitch(&pitch));

		return pitch;
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

	void AudioAPI::GetCurrentPlayTime(FMOD::Channel* channel, unsigned int* timeStamp)
	{
		channel->getPosition(timeStamp, FMOD_TIMEUNIT_PCM);
	}

	void AudioAPI::SetCurrentPlayTime(FMOD::Channel* channel, unsigned int timeStamp)
	{
		channel->setPosition(timeStamp, FMOD_TIMEUNIT_PCM);
	}


	void AudioAPI::PlaySoundFromTime(FMOD::Channel* channel, float startPoint)
	{
		UINTLL dsp_clock = GetDSPClock(channel);
		FMOD::Sound* sound; channel->getCurrentSound(&sound);

		unsigned int length; sound->getLength(&length, FMOD_TIMEUNIT_MS);

		channel->setDelay(dsp_clock + static_cast<UINTLL>(m_sample_rate * startPoint), length, false);
	}

	void AudioAPI::PlaySoundUntilTime(FMOD::Channel* channel, float endPoint)
	{
		UINTLL dsp_clock = GetDSPClock(channel);

		channel->setDelay(dsp_clock, dsp_clock + static_cast<UINTLL>(m_sample_rate * endPoint), false);
	}

	void AudioAPI::PlaySoundBetweenTimes(FMOD::Channel* channel, float startPoint, float endPoint)
	{
		( channel );
		( startPoint );
		( endPoint );
	}

	void AudioAPI::PauseAll()
	{
		m_master_channel_group->setPaused(true);
	}

	void AudioAPI::ResumeAll()
	{
		m_master_channel_group->setPaused(false);
	}

	void AudioAPI::StopAndResetAll(std::vector<std::tuple<uint32_t, Sound>> const& assets)
	{
		//m_master_channel_group->stop();

		for (auto& channel : m_channels)
		{
			channel->stop();
		}

		for (auto& asset : assets)
		{
			Sound& sound = const_cast<Sound&>(std::get<1>(asset));
			sound.m_played = false;
		}

		m_channels.clear();
	}

	void AudioAPI::PlayBGM()
	{
		if (BGM.m_channel == nullptr)
		{
			m_FMOD_system->playSound(BGM.m_sound_sample, NULL, false, &BGM.m_channel);
			SetVolume(BGM.m_channel, 0.2f);
		}

		BGM.m_played = true;
	}

	//void AudioAPI::PauseBGM()
	//{

	//}

	//void AudioAPI::ResumeBGM()
	//{
	//}

	void AudioAPI::StopBGM()
	{
		if (BGM.m_channel != nullptr)
		{
			BGM.m_channel->stop();
			BGM.m_channel = nullptr;
		}
	}

	void AudioAPI::ToggleDSPEffects(bool toggle)
	{
		dsp_echo->setBypass(!toggle);
	}

	void AudioAPI::AddEcho(FMOD::Channel* channel)
	{
		ErrorCheck(channel->addDSP(0, dsp_echo));
	}

	void AudioAPI::AddFadeIn(FMOD::Channel* channel, float fadeInTime, bool& fade)
	{
		( fade );
		UINTLL dsp_clock = GetDSPClock(channel);

		channel->setPaused(true);

		SetVolume(channel, 0.f);
		ErrorCheck(channel->setDelay(0, 0, false)); // delay to resume sound
		ErrorCheck(channel->addFadePoint(dsp_clock, 0.f));
		ErrorCheck(channel->addFadePoint(dsp_clock + static_cast<UINTLL>((m_sample_rate * fadeInTime)), 1.f));

		channel->setPaused(false);
	}

	void AudioAPI::AddFadeOut(FMOD::Channel* channel, float fadeOutTime, bool& fade)
	{
		( fade );
		UINTLL dsp_clock = GetDSPClock(channel);

		float volume = GetVolume(channel);

		ErrorCheck(channel->addFadePoint(dsp_clock, volume));
		ErrorCheck(channel->addFadePoint(dsp_clock + static_cast<UINTLL>((m_sample_rate * fadeOutTime)), 0.f));
		ErrorCheck(channel->setDelay(0, dsp_clock + static_cast<UINTLL>((m_sample_rate * fadeOutTime)), false)); // delay to stop sound
	}

	void AudioAPI::RemoveFadeIn(FMOD::Channel* channel, float fadeInPoint)
	{
		UINTLL dsp_clock = GetDSPClock(channel);

		channel->removeFadePoints(dsp_clock, static_cast<UINTLL>(dsp_clock + fadeInPoint));
	}

	void AudioAPI::RemoveFadeOut(FMOD::Channel* channel, float fadeOutPoint)
	{
		UINTLL dsp_clock = GetDSPClock(channel);

		channel->removeFadePoints(dsp_clock, static_cast<UINTLL>(dsp_clock + fadeOutPoint));
	}



	void AudioAPI::GetFadePoints(FMOD::Channel* channel, unsigned int* points)
	{
		channel->getFadePoints(points, 0, 0);
	}

	int AudioAPI::GetSampleRate()
	{
		return m_sample_rate;
	}

	UINTLL AudioAPI::GetDSPClock(FMOD::Channel* channel)
	{
		UINTLL dsp_clock;
		ErrorCheck(channel->getDSPClock(0, &dsp_clock));

		return dsp_clock;
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