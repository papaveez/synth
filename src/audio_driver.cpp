#include "audio_driver.h"
#include <iostream>


void 
Audio::data_callback(ma_device* device, void* output, 
                           const void* input, ma_uint32 frame_count) 
{
    Audio* audio_driver = (Audio*) device->pUserData;

    float* out = (float*) output;
    float* buf = nullptr;

    while (audio_driver->buffer_queue.try_dequeue(buf)) {}
    if (buf == nullptr) return;

    std::memcpy(out, buf, sizeof(float)*frame_count);
    audio_driver->free_list.enqueue(buf);
}
 

Audio::Audio(size_t sample_rate, size_t frame_count, size_t num_bufs) :
    sample_rate(sample_rate),
    frame_count(frame_count),
    cfg(ma_device_config_init(ma_device_type_playback)),
    buffers(new float[num_bufs*frame_count]),
    buffer_queue(num_bufs),
    free_list(num_bufs)
{
    for (size_t i=0;i<num_bufs;++i) {
        free_list.enqueue(buffers +i*frame_count);
    }

    cfg.playback.format = ma_format_f32;
    cfg.playback.channels = 1;

    cfg.sampleRate = sample_rate;
    cfg.periodSizeInFrames = frame_count;
    cfg.dataCallback = data_callback;
    cfg.pUserData = this;

    if (ma_device_init(NULL, &cfg, &device) != MA_SUCCESS) {
        std::cerr << "Error: could not initialise miniaudio" << std::endl;
        std::exit(1);
    }
}

Audio::Audio() : Audio(44100, 64, 256) {}

Audio::~Audio() {
    ma_device_uninit(&device);
    free(buffers);
}

void Audio::start() {
    ma_device_start(&device);
}

void Audio::stop() {
    ma_device_stop(&device);
}

void Audio::push_buffer(const float* buf) {
    float* in_queue;
    free_list.wait_dequeue(in_queue);

    std::memcpy(in_queue, buf, sizeof(float)*frame_count);
    buffer_queue.enqueue(in_queue);
}

size_t Audio::get_sample_rate() const { return sample_rate; }

