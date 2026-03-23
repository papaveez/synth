#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H
#include "miniaudio.h"


class AudioDriver {
private:
    const size_t sample_rate;
    const size_t frame_count;


    ma_device_config cfg;
    ma_device device;

    static void 
        data_callback(ma_device*, void* output, const void* input, 
                      ma_uint32 frame_count);
 
    void (*callback)(float*, const size_t) = nullptr;

public:
    AudioDriver(size_t sample_rate, size_t frame_count, void(*callback)(float*, const size_t));
    AudioDriver();
    ~AudioDriver();

    void start();
    void stop();

    size_t get_sample_rate() const;
    size_t ring_buffer_size() const;
};

#endif
