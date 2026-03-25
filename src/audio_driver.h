#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H
#include "miniaudio.h"

#include "readerwriterqueue.h"

class BufferMonitor {
private:
    size_t frame_count;
    size_t buffer_count;

    moodycamel::ReaderWriterQueue<float*> buffer_list;
    moodycamel::ReaderWriterQueue<float*> free_list;

    std::vector<float> buffers;
    std::vector<float> ring_buffer;
    size_t ring_idx = 0;

    void copy_to_ringbuffer(float* buf);

public:
    BufferMonitor(size_t frame_count, size_t num_samples);

    void push_buffer(const float* buffer);

    class Consumer {
        private:
            BufferMonitor& parent;
            friend class BufferMonitor;

            Consumer (BufferMonitor& p);
        public:
            void update();
            void read_out(float* out, size_t num_bufs);
            size_t get_buffer_size() const;
    };

    Consumer get_consumer();
};

class Audio {
private:
    const size_t sample_rate;
    const size_t frame_count;

    ma_device_config cfg;
    ma_device device;


    static void 
        data_callback(ma_device*, void* output, const void* input, 
                      ma_uint32 frame_count);
 
    void (*process)(float* buffer, const size_t num_frames) = nullptr;
    std::unique_ptr<BufferMonitor> monitor;

public:
    Audio(size_t sample_rate, size_t frame_count);
    ~Audio();

    void set_processor(void (*p)(float*, const size_t));

    BufferMonitor::Consumer get_monitor_consumer(size_t buffer_count);

    void start();
    void stop();

    size_t get_sample_rate() const;
};

#endif
