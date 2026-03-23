#define MINIAUDIO_IMPLEMENTATION
#include "audio_driver.h"

#include "midi.h"
#include "synth.h"

const size_t sample_rate = 44100;
const size_t num_frames = 64;
const size_t num_buffers = 128;
Synth s(88, 21, 27.5f, sample_rate);

void process(float* buf, const size_t num_frames) {
    s.synthesise(buf, num_frames);
}

int main() {
    auto& midi = MidiHandler::get_instance();
    midi.init(0);

    EchoKeyboard keyboard;
    Audio a(sample_rate, num_frames, num_buffers);

    a.start();


    float* buffer = new float[num_frames];
    while (!midi.is_done()) {
        midi.update(&s);

        s.synthesise(buffer, num_frames);
        a.push_buffer(buffer);
    }

    free(buffer);
}
