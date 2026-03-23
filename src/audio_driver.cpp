#include "audio_driver.h"
#include <iostream>


void 
AudioDriver::data_callback(ma_device* device, void* output, 
                           const void* input, ma_uint32 frame_count) 
{
    if (input == nullptr) return;
    const float* in = (const float*) input;
    float* out = (float*) output;

    AudioDriver* audio_driver = (AudioDriver*) device->pUserData;

    if (audio_driver->callback != nullptr) {
        audio_driver->callback(out, frame_count);
    }
}
 

AudioDriver::AudioDriver(size_t sample_rate, size_t frame_count,  void (*callback)(float*, const size_t)) :
    callback(callback),
    sample_rate(sample_rate),
    frame_count(frame_count),
    cfg(ma_device_config_init(ma_device_type_duplex))
{
    // initialise ma device
    cfg.capture.format = ma_format_f32;
    cfg.capture.channels = 1;
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

AudioDriver::AudioDriver() : AudioDriver(44100, 64, nullptr) {}

AudioDriver::~AudioDriver() {
    ma_device_uninit(&device);
}

void AudioDriver::start() {
    ma_device_start(&device);
}

void AudioDriver::stop() {
    ma_device_stop(&device);
}

size_t AudioDriver::get_sample_rate() const { return sample_rate; }

