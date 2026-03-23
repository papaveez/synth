#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <cstdlib>

struct Keyboard {
    virtual void note_on(uint8_t key_midi_id, uint8_t velocity) {};
    virtual void note_off(uint8_t key_midi_id) {};

    virtual void sustain_on()  {};
    virtual void sustain_off() {};

    virtual ~Keyboard() = default;

    virtual void synthesise(float* buffer, const size_t num_frames) {};
};


struct EchoKeyboard : Keyboard {
    void note_on(uint8_t, uint8_t) override;
    void note_off(uint8_t) override;
    
    void sustain_on() override;
    void sustain_off() override;
};
#endif
