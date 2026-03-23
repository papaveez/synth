#define MINIAUDIO_IMPLEMENTATION
#include "audio_driver.h"

#include "midi.h"
#include "synth.h"


const size_t sample_rate = 44100;
const size_t num_frames = 64;
Synth s(88, 21, 27.5f, sample_rate);

void process(float* buf, const size_t num_frames) {
    s.synthesise(buf, num_frames);
}

int main() {

    auto& midi = MidiHandler::get_instance();
    midi.init(0);

    EchoKeyboard keyboard;
    AudioDriver a(44100, 64, &process);

    a.start();

    while (!midi.is_done()) {
        midi.update(&s);
    }
}
