#include "AudioSystem.h"

using namespace DirectX;
AudioSystem::AudioSystem()
{
	AUDIO_ENGINE_FLAGS flag = AudioEngine_Default;

#ifdef _DEBUG
	flag = flag | AudioEngine_Debug;
#endif

	m_audio_engine = std::make_unique<AudioEngine>(flag);
}

bool AudioSystem::loadMusic(int trackNO, const wchar_t* filename, float volume)
{
	if (m_music_instances[trackNO]) m_music_instances[trackNO].reset();

	m_musics[trackNO] = std::make_unique<SoundEffect>(m_audio_engine.get(), filename);

	m_music_volume_knobs[trackNO] = volume;

	return true;
}

bool AudioSystem::loadSound(const wchar_t* filename, float volume)
{
	for (auto& file : m_sound_instances)
	{
		for (auto& sound : file)
		{
			sound.reset();
		}
	}
	m_wave_bank = std::make_unique<WaveBank>(m_audio_engine.get(), filename);
	return true;
}

void AudioSystem::unloadMusic(int trackNO)
{
}


AudioSystem::~AudioSystem()
{
}
