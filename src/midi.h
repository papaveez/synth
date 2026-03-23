#ifndef MIDI_H
#define MIDI_H

#include <cstdlib>
#include <csignal>
#include <cstdlib>
#include <atomic>

#include "RtMidi.h"

#include "keyboard.h"



enum class MidiStatus : uint8_t {
    NoteOff = 0x80,
    NoteOn  = 0x90,
    ControlChange = 0xB0
};


// singleton MIDI handler class
class MidiHandler {
    static std::atomic<bool> done;
    bool ready = false;

    std::unique_ptr<RtMidiIn> midi_in;
    std::vector<unsigned char> message;
    MidiHandler() = default;
    MidiHandler(const MidiHandler&) = delete;
    MidiHandler& operator=(const MidiHandler&) = delete;

public:
    static MidiHandler& get_instance();
    static void finish(int);

    bool is_done();
    void init(int port);
    void update(Keyboard* keyboard);
};


#endif
