#include "synth.h"

void Param::set(float v) {
    value.store(v, std::memory_order_relaxed);
}

float Param::get() const {
    return value.load(std::memory_order_relaxed);
}


// Oscillator

void Oscillator::set_frequency(float f) { freq = f; }

void Oscillator::reset() { phase = 0.0f; }

float Oscillator::sample(const float dt) {
    phase += freq*dt;
    if (phase > 1) phase -= 1;
    return std::sin(2.0f *M_PI*phase);
}


// Voice
Voice::Voice(float freq) {
    osc.set_frequency(freq);
}


void Voice::note_on(uint8_t vel) {
    active = true;
    pressed = true;
    velocity = vel;
    time_pressed_s = 0.0f;
    time_release_s = 0.0f;
    osc.reset();
}


void Voice::note_off() {
    pressed = false;
}


void Voice::update_timers(const float dt) {
    if (pressed) time_pressed_s += dt;
    else time_release_s += dt;
}



float Envelope::get_coeff(const Voice& voice) {
    float coeff;
    float attack = params.attack.get();
    if (voice.time_pressed_s < attack) {
        coeff = std::clamp(voice.time_pressed_s/attack, 0.0f, 1.0f);
    } else {
        float progress = voice.time_pressed_s - attack;
        coeff = 1.0f - std::clamp(progress/params.decay.get(), 0.0f, 1.0f);
    }
    if (!voice.pressed) {
        coeff *= 1.0f - std::clamp(voice.time_release_s/params.release.get(), 0.0f, 1.0f);
    }
    return coeff;
}


bool Envelope::is_done(const Voice& voice) {
    if (voice.pressed) {
        return voice.time_pressed_s > params.attack.get() + params.decay.get();
    } else {
        return voice.time_release_s > params.release.get();
    }
}



Synth::Synth(uint8_t num_keys, uint8_t min_midi_id, float low_freq, size_t sample_rate) : 
    min_midi_id(min_midi_id), 
    sample_rate(sample_rate), 
    period_s(1.0f/(float)sample_rate)
{
    float freq = low_freq;
    for (int i=0;i<num_keys;++i) {
        voices.push_back(Voice(freq));
        freq = freq * std::pow(2.0f, 1.0f/12.0f);
    }
}

bool Synth::midi_id_in_range(uint8_t midi_id) {
    return midi_id >= min_midi_id && midi_id < min_midi_id + voices.size();
}

void Synth::note_on(uint8_t midi_id, uint8_t velocity) {
    if (!midi_id_in_range(midi_id))
        return;

    size_t idx = midi_id - min_midi_id;
    voices[idx].note_on(velocity);
}


void Synth::note_off(uint8_t midi_id) {
    if (!midi_id_in_range(midi_id))
        return;

    size_t idx = midi_id - min_midi_id;
    voices[idx].note_off();
}

void Synth::synthesise(float* buf, const size_t num_frames) {
    for (int i=0;i<num_frames;++i) {
        buf[i] = 0.0f;
        float count = 0.0f;
        for (auto& v : voices) {
            if (!v.active) continue;
            v.update_timers(period_s);
            float coeff = e.get_coeff(v);
            if (e.is_done(v)) v.active = false;
            buf[i] += coeff*v.osc.sample(period_s);
            count += 1.0f;
        }
        buf[i] /= count;
    }
}
