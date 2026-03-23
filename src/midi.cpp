#include "midi.h"

std::atomic<bool> MidiHandler::done = false;

MidiHandler& MidiHandler::get_instance() {
    static MidiHandler instance;
    return instance;
}

void MidiHandler::finish(int) {
    done = true;
}

bool MidiHandler::is_done() {
    return done;
}

void MidiHandler::init(int port) {
    try {
        midi_in = std::make_unique<RtMidiIn>();
    } catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    unsigned int n_ports = midi_in->getPortCount();
    if (port >= n_ports) {
        std::cerr << "Desired MIDI device not available. exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    midi_in->openPort(port);
    midi_in->ignoreTypes(false, false, false);
    std::signal(SIGINT, MidiHandler::finish);
    ready = true;
}


void MidiHandler::update(Keyboard* keyboard) {
    if (done || !ready) return;

    float time_stamp = midi_in->getMessage(&message);
    if (message.size() < 3) return;
    uint8_t status = message[0];
    uint8_t data_1 = message[1];
    uint8_t data_2 = message[2];


    using enum MidiStatus;

    switch (status & 0xF0) {
        case ((uint8_t) NoteOn):
            keyboard->note_on(data_1, data_2);
            break;
        case ((uint8_t) NoteOff):
            keyboard->note_off(data_1);
            break;
        case((uint8_t) ControlChange):
            if ((data_1&0xF0) == 0x40) {
                if (data_2 < 64) keyboard->sustain_off();
                else if (data_2 > 63) keyboard->sustain_on();

            }
            break;
    }
}
