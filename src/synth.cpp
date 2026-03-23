#include "synth.h"
#include <cstdint>
#include <iostream>


Note::Note(float fundamental) :
    fundamental(fundamental)
{}



float Note::sample(float delta_time_s) {
    float amplitude = 0.3f;
    // pure sine wave
    phase += delta_time_s * fundamental;
    time_s += delta_time_s;
    if (phase > 1) phase -=1 ;

    float out = amplitude*std::sin(2.0f*M_PI*phase);

    if (!pressed) deactivate_flag = true;
    return out;
}



Synth::Synth(uint8_t num_keys, uint8_t min_midi_id, float low_freq, size_t sample_rate) : 
    min_midi_id(min_midi_id), sample_rate(sample_rate), period_s(1.0f/(float)sample_rate)
{
    notes.reserve(num_keys);
    for (int i=0;i<num_keys;++i) {
        float freq = low_freq * std::pow(2.0f, (float) i /12.0f);
        notes.push_back(Note(freq));
    }
}

bool Synth::midi_id_in_range(uint8_t midi_id) {
    return midi_id >= min_midi_id && midi_id < min_midi_id + notes.size();
}

void Synth::note_on(uint8_t midi_id, uint8_t velocity) {
    if (!midi_id_in_range(midi_id))
        return;

    size_t idx = midi_id - min_midi_id;

    notes[idx].pressed = true;
    notes[idx].phase = 0.0f;
    notes[idx].time_s = 0.0f;
    notes[idx].velocity = velocity;

    if (!notes[idx].list_pos.has_value()) {
        active_notes.push_back(idx);
        notes[idx].list_pos = std::prev(active_notes.end());
    }
}


void Synth::note_off(uint8_t midi_id) {
    if (!midi_id_in_range(midi_id))
        return;

    size_t idx = midi_id - min_midi_id;
    notes[idx].pressed = false;
}

void Synth::synthesise(float* buf, const size_t num_frames) {
    for (int i=0;i<num_frames;++i) {
        buf[i] = 0.0f;
        for (auto it=active_notes.begin(); it != active_notes.end(); ) {
            auto curr = it++;
            Note& note = notes[*curr];
            buf[i] += note.sample(period_s);

            if (note.deactivate_flag) {
                active_notes.erase(note.list_pos.value());
                note.deactivate_flag = false;
                note.list_pos = {};
            }
        }
    }
}
