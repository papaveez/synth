#define MINIAUDIO_IMPLEMENTATION
#include "audio_driver.h"

#include "midi.h"
#include "synth.h"
#include "raylib.h"

const size_t sample_rate = 44100;
const size_t num_frames = 64;
const size_t num_buffers = 128;
const float width = 1280.0f;
const float height = 720.0f;
const float scale = 50.0f;

Synth s(88, 21, 27.5f, sample_rate);

void process(float* buf, const size_t num_frames) {
    s.synthesise(buf, num_frames);
}

int main() {
    auto& midi = MidiHandler::get_instance();
    midi.init(0);

    Audio a(sample_rate, num_frames);

    a.set_processor(process);

    BufferMonitor::Consumer cons = a.get_monitor_consumer(64);

    InitWindow(1280, 720, "Synth");
    SetTargetFPS(60);

    std::vector<float> samples (64*cons.get_buffer_size());
    std::vector<Vector2> points(samples.size());

    a.start();

    while (!(midi.is_done() || WindowShouldClose())) {
        midi.update(&s);

        cons.update();
        cons.read_out(samples.data(), samples.size());


        for (int i=0;i<samples.size();++i) {
            points[i] = Vector2 {
                width * (float) i/ (float)samples.size(),
                height/2.0f - scale*samples[i]
            };
        }

        BeginDrawing();
            ClearBackground(BLACK);
            DrawSplineLinear(points.data(), points.size(), 2, BLUE);
        EndDrawing();
    }
}
