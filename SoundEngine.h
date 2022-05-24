#pragma once

#include <unordered_map>

#include <soloud.h>
#include <soloud_wav.h>

enum SoundEnum
{
    SFX_AMBIENCE,
    SFX_BUY,
    SFX_SELL,
    SFX_PLOT_PLACE,
    SFX_POISON,
    SFX_PLANT,
    SFX_EGGPLANT_AMBIENCE,
    SFX_EGGPLANT_PICKUP,
    SFX_STEAL,
    SFX_SHOVEL,
    SFX_WATERING,
    SFX_NO,
    SFX_FERTILIZE
};

struct SoundEngine
{
    SoLoud::Soloud engine{};

    // Wav Components
    SoLoud::Wav wav_sfx_ambience;
    SoLoud::Wav wav_sfx_buy;
    SoLoud::Wav wav_sfx_sell;
    SoLoud::Wav wav_sfx_plot_place;
    SoLoud::Wav wav_sfx_poison;
    SoLoud::Wav wav_sfx_plant;
    SoLoud::Wav wav_sfx_eggplant_ambience;
    SoLoud::Wav wav_sfx_eggplant_pickup;
    SoLoud::Wav wav_sfx_steal;
    SoLoud::Wav wav_sfx_shovel;
    SoLoud::Wav wav_sfx_watering;
    SoLoud::Wav wav_sfx_no;
    SoLoud::Wav wav_sfx_fertilize;

    std::unordered_map<SoundEnum, SoLoud::Wav> sound_wav_map = {
        {SFX_AMBIENCE, wav_sfx_ambience},
        {SFX_BUY, wav_sfx_buy},
        {SFX_SELL, wav_sfx_sell},
        {SFX_PLOT_PLACE, wav_sfx_plot_place},
        {SFX_POISON, wav_sfx_poison},
        {SFX_PLANT, wav_sfx_plant},
        {SFX_EGGPLANT_AMBIENCE, wav_sfx_eggplant_ambience},
        {SFX_EGGPLANT_PICKUP, wav_sfx_eggplant_pickup},
        {SFX_STEAL, wav_sfx_steal},
        {SFX_SHOVEL, wav_sfx_shovel},
        {SFX_WATERING, wav_sfx_watering},
        {SFX_NO, wav_sfx_no},
        {SFX_FERTILIZE, wav_sfx_fertilize},
    };

    std::unordered_map<SoundEnum, const char*> sound_file_map = {
        {SFX_AMBIENCE, "sound/sfx/ambience.wav"},
        {SFX_BUY, "sound/sfx/buy.wav"},
        {SFX_SELL, "sound/sfx/sell.wav"},
        {SFX_PLOT_PLACE, "sound/sfx/plot-placement.wav"},
        {SFX_POISON, "sound/sfx/poison.wav"},
        {SFX_PLANT, "sound/sfx/plant.wav"},
        {SFX_EGGPLANT_AMBIENCE, "sound/sfx/eggplant-ambience.wav"},
        {SFX_EGGPLANT_PICKUP, "sound/sfx/eggplant-pickup.wav"},
        {SFX_STEAL, "sound/sfx/steal.wav"},
        {SFX_SHOVEL, "sound/sfx/shovel.wav"},
        {SFX_WATERING, "sound/sfx/watering.wav"},
        {SFX_NO, "sound/sfx/no.wav"},
        {SFX_FERTILIZE, "sound/sfx/plant.wav"},
    };
    
    void Init()
    {
        engine.init();
        for (auto & [soundEnum, wav] : sound_wav_map) {
            wav.load(sound_file_map[soundEnum]);
            if (soundEnum == SFX_AMBIENCE || soundEnum == SFX_EGGPLANT_AMBIENCE) wav.setLooping(true);
        }
    }
    void DeInit() { engine.deinit(); }

    void Play(const SoundEnum sound) { engine.play(sound_wav_map[sound]); }


};