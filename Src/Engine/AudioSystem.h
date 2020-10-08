#pragma once

#ifdef _DEBUG
#pragma comment(lib, "DirectXTKAudioWin8.lib")
#else
#pragma comment(lib, "DirectXTKAudioWin8.lib")
#endif

#include <Audio.h>
#include <unordered_map>
#include <memory>
#include <array>


class AudioSystem final
{
private:
	static constexpr float MAX_VOLUME = 1.0f;
	static constexpr unsigned int MUSIC_FILE_MAX = 32;
	static constexpr unsigned int WAVE_SOUND_MAX = 8;
	static constexpr unsigned int WAVE_FILE_MAX = 128;

private:
	std::unique_ptr<DirectX::AudioEngine> m_audio_engine = nullptr;
	std::unique_ptr<DirectX::WaveBank> m_wave_bank = nullptr;

	std::array<float, MUSIC_FILE_MAX> m_music_volume_knobs;
	std::array<std::unique_ptr<DirectX::SoundEffect>, MUSIC_FILE_MAX> m_musics;
	std::array<std::unique_ptr<DirectX::SoundEffectInstance>, MUSIC_FILE_MAX> m_music_instances;

	std::array<float, WAVE_SOUND_MAX> m_sound_volume_knobs;
	std::array<std::array<std::unique_ptr<DirectX::SoundEffectInstance>, WAVE_FILE_MAX>, WAVE_SOUND_MAX> m_sound_instances;
public:
	AudioSystem();

	// load / unload 
	bool loadMusic(int trackNO, const wchar_t* filename, float volume = 0.5f);
	bool loadSound(const wchar_t* filename, float volume = 0.5f);

	void unloadMusic(int trackNO);


	// Music(BGM)
	inline void playMusic(int trackNO, bool isLoop)
	{
		if (m_music_instances[trackNO]) return;
		m_music_instances[trackNO] = m_musics[trackNO]->CreateInstance();
		m_music_instances[trackNO]->Play(isLoop);
		m_music_instances[trackNO]->SetVolume(m_music_volume_knobs[trackNO]);
	}

	inline void stopMusic(int trackNO) 
	{ 
		if (!m_music_instances[trackNO]) return;
		m_music_instances[trackNO]->Stop();
	}
	inline void pauseMusic(int trackNO)
	{
		if (!m_music_instances[trackNO]) return;
		m_music_instances[trackNO]->Pause();
	}
	inline void resumeMusic(int trackNO)
	{
		if (!m_music_instances[trackNO]) return;
		m_music_instances[trackNO]->Resume();
	}
	inline void setMusicVolume(int trackNO, float volume)
	{
		if (!m_music_instances[trackNO]) return;
		m_music_instances[trackNO]->SetVolume(volume);
	}
	inline bool isLooped(int trackNO)
	{
		return m_music_instances[trackNO]->IsLooped();
	}
	inline bool isMusicInUse(int trackNO)
	{
		return m_musics[trackNO]->IsInUse();
	}

	// Sound(SFX)
	void playSound(int trackNO)
	{
		for (auto& sound : m_sound_instances[trackNO])
		{
			if (sound)
			{
				DirectX::SoundState state = sound->GetState();
				if (state != DirectX::SoundState::STOPPED) continue;
			}

			sound = m_wave_bank->CreateInstance(trackNO);
			if (sound)
			{
				sound->SetVolume(m_sound_volume_knobs[trackNO]);
				sound->Play();
				break;
			}
		}
	}

	inline void setSoundVolume(int trackNO, float volume)
	{
		m_sound_volume_knobs[trackNO] = volume;
	}

	~AudioSystem();
};