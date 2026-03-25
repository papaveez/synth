#include "audio_driver.h"
#include <iostream>


BufferMonitor::BufferMonitor(size_t frame_count, size_t buffer_count) :
    frame_count(frame_count),
    buffer_count(buffer_count),
    buffers(frame_count*buffer_count),
    free_list(buffer_count),
    buffer_list(buffer_count),
    ring_buffer(buffer_count*frame_count)
{
    float* ptr = buffers.data();
    for (int i=0;i<buffer_count;++i) {
        free_list.enqueue(ptr);
        ptr = ptr + frame_count;
    }
}


void BufferMonitor::copy_to_ringbuffer(float* buf) {
    for (size_t j=0;j<frame_count;++j) {
        ring_buffer[ring_idx] = *buf;
        buf++;
        ring_idx = (ring_idx+1)%ring_buffer.size();
    }
}


void BufferMonitor::push_buffer(const float* buffer) {
    float* dest;

    if (!free_list.try_dequeue(dest)) return;

    std::memcpy(dest, buffer, sizeof(float)*frame_count);
    buffer_list.enqueue(dest);
}


BufferMonitor::Consumer::Consumer(BufferMonitor& p) : parent(p) {}


void BufferMonitor::Consumer::update() {
    float* buf;
    while (parent.buffer_list.try_dequeue(buf)) {
        parent.copy_to_ringbuffer(buf);
        parent.free_list.enqueue(buf);
    }
}

void BufferMonitor::Consumer::read_out(float* out, size_t num_samples) {
    num_samples = std::min(num_samples, parent.ring_buffer.size());
    std::vector<float>& ring_buf = parent.ring_buffer;
    size_t ring_buf_size = ring_buf.size();

    int ring_idx = ((int) parent.ring_idx - 1)%ring_buf_size;
    // get latest buffers
    for (int idx=num_samples-1;idx>=0;--idx) {
        out[idx] = ring_buf[ring_idx];
        ring_idx = (ring_idx-1)%ring_buf_size;
    }
}


size_t BufferMonitor::Consumer::get_buffer_size() const {
    return parent.frame_count;
}


BufferMonitor::Consumer BufferMonitor::get_consumer() {
    return BufferMonitor::Consumer(*this);
}

void 
Audio::data_callback(ma_device* device, void* output, 
                    const void* input, ma_uint32 frame_count) 
{
    Audio* audio_driver = (Audio*) device->pUserData;

    float* out = (float*) output;

    if (audio_driver->process != nullptr) 
        audio_driver->process(out, frame_count);
    
    if (audio_driver->monitor)
        audio_driver->monitor->push_buffer(out);
}
 

Audio::Audio(size_t sample_rate, size_t frame_count) :
    sample_rate(sample_rate),
    frame_count(frame_count),
    cfg(ma_device_config_init(ma_device_type_playback))
{
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


Audio::~Audio() {
    ma_device_uninit(&device);
}


void Audio::set_processor(void (*p)(float*, const size_t)) {
    process = p;
}

BufferMonitor::Consumer Audio::get_monitor_consumer(size_t buffer_count) {
    monitor = std::make_unique<BufferMonitor>(frame_count, buffer_count);
    return monitor->get_consumer();
}


void Audio::start() {
    ma_device_start(&device);
}

void Audio::stop() {
    ma_device_stop(&device);
}


size_t Audio::get_sample_rate() const { return sample_rate; }
