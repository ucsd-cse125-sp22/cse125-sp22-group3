#pragma once

#include <unordered_map>

#include <soloud.h>
#include <soloud_wav.h>

enum SoundEnum
{
    SFX_AMBIENCE,
    SFX_BUY,
    SFX_SELL,
    SFX_PLOT_PLACE
};

struct SoundEngine
{
    SoLoud::Soloud engine{};

    // Wav Components
    SoLoud::Wav wav_sfx_ambience;
    SoLoud::Wav wav_sfx_buy;
    SoLoud::Wav wav_sfx_sell;
    SoLoud::Wav wav_sfx_plot_place;

    std::unordered_map<SoundEnum, SoLoud::Wav> sound_wav_map = {
        {SFX_AMBIENCE, wav_sfx_ambience},
        {SFX_BUY, wav_sfx_buy},
        {SFX_SELL, wav_sfx_sell},
        {SFX_PLOT_PLACE, wav_sfx_plot_place}
    };

    std::unordered_map<SoundEnum, const char*> sound_file_map = {
        {SFX_AMBIENCE, "sound/sfx/ambience.wav"},
        {SFX_BUY, "sound/sfx/buy.wav"},
        {SFX_SELL, "sound/sfx/sell.wav"},
        {SFX_PLOT_PLACE, "sound/sfx/plot-placement.wav"}
    };
    
    void Init()
    {
        engine.init();
        for (auto & [soundEnum, wav] : sound_wav_map) {
            wav.load(sound_file_map[soundEnum]);
        }
    }
    void DeInit() { engine.deinit(); }

    void Play(const SoundEnum sound) { engine.play(sound_wav_map[sound]); }


};