#include "keyboard.h"

#include <iostream>

void EchoKeyboard::note_on(uint8_t key_midi_id, uint8_t velocity) {
    std::cout << "[NOTE_ON] key="  << (int) key_midi_id 
              << ", velocity=" <<(int)velocity<<std::endl;
}


void EchoKeyboard::note_off(uint8_t key_midi_id) {
    std::cout << "[NOTE_OFF] key=" << (int) key_midi_id 
              << std::endl;
}


void EchoKeyboard::sustain_on() {
    std::cout << "[SUSTAIN_ON]" << std::endl;
}


void EchoKeyboard::sustain_off() {
    std::cout << "[SUSTAIN_OFF]" << std::endl;
}

