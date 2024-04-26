#ifndef COMMON_DISPLAY_IFACE_H_
#define COMMON_DISPLAY_IFACE_H_

#include <cstdint>
#include <functional>


struct vo_size_info_t
{
    // Runtime size
    uint32_t win_w{};
    uint32_t win_h{};

    // Frame Buffer size
    uint32_t frame_w{};
    uint32_t frame_h{};
};

struct i_display
{
    using irq_callback_t = void(*)(int);

    i_display() {}
    virtual ~i_display() {}

    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void set_flip_rate(int rate_hz)
    {
        fliprate_hz = rate_hz;
        frame_time_ms_ = uint32_t(1000.0f / rate_hz);
    }

    virtual vo_size_info_t get_info() = 0;

    virtual void register_vo_buffers(
        uint32_t        start_idx,
        void const    **addresses,
        uint32_t        num_buffers,
        void const     *attributes) = 0;

    virtual void submit_flip(
        int             buf_idx,
        uint32_t        flip_mode,
        uint64_t        flip_arg,
        bool            eop) = 0;

    virtual void set_irq_callback(irq_callback_t cb)
    {
        cb_irq_ = cb;
    }

    int                         fliprate_hz{ 60 };
    uint32_t                    frame_time_ms_{ 16 };
    std::function<void(int)>    cb_irq_{};
};

#endif // COMMON_DISPLAY_IFACE_H_
