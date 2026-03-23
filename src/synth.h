#ifndef SYNTH_H
#define SYNTH_H

#include "keyboard.h"
#include <cstdint>
#include <list>


struct Note {
    const float fundamental;
    std::optional<std::list<uint8_t>::iterator> list_pos{};

    float phase = 0.0f;
    float time_s = 0.0f;

    uint8_t velocity = 0;
    bool pressed = false;
    bool deactivate_flag = false;

    Note(float fundamental);

    float sample(float delta_time_s);
};


struct Envelope {
    float attack_s;
    float decay_s;
    float release_s;

    float sample(const Note& note);
};


struct Compressor {
    void compress(float* buf, const size_t num_frames);
};


struct Synth : Keyboard {
    const uint8_t min_midi_id;
    const size_t sample_rate;
    const float period_s;

    std::vector<Note> notes;
    std::list<uint8_t> active_notes;

    Synth(uint8_t num_keys, uint8_t min_midi_id, float low_freq, size_t sample_rate);

    bool midi_id_in_range(uint8_t midi_id);

    void note_on(uint8_t, uint8_t) override;
    void note_off(uint8_t) override;

    void synthesise(float* buf, const size_t num_frames) override;
};

#endif
