#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H
#include "miniaudio.h"

#include "readerwriterqueue.h"

class Audio {
private:
    const size_t sample_rate;
    const size_t frame_count;

    ma_device_config cfg;
    ma_device device;


    float* buffers;
    moodycamel::ReaderWriterQueue<float*> buffer_queue;
    moodycamel::BlockingReaderWriterQueue<float*> free_list;

    static void 
        data_callback(ma_device*, void* output, const void* input, 
                      ma_uint32 frame_count);
 

public:
    Audio(size_t sample_rate, size_t frame_count, size_t num_bufs);
    Audio();
    ~Audio();

    void start();
    void stop();

    void push_buffer(const float* buf);

    size_t get_sample_rate() const;
};

#endif
