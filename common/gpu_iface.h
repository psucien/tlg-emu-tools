#ifndef COMMON_GPU_IFACE_H_
#define COMMON_GPU_IFACE_H_

#include <cstdint>
#include <functional>


struct i_gpu
{
    using irq_callback_t = void(*)(int);

    i_gpu() {}
    virtual ~i_gpu() {}

    virtual void init() = 0;
    virtual void destroy() = 0;

    virtual void submit_gfxq(
        uint32_t  count,
        void        const   *dcb_addresses[],
        uint32_t    const   *dcb_sizes,
        void        const   *ccb_addresses[],
        uint32_t    const   *ccb_sizes) = 0;

    virtual void submit_acq(
        uint32_t            vqid,
        void       * const  cb,
        uint32_t            size) = 0;

    virtual void submit_done(void) = 0;
    virtual void wait_submissions_allowed() = 0;

    virtual void set_irq_callback(irq_callback_t cb) { cb_irq_ = cb; }

    std::function<void(int)>    cb_irq_{};
};

#endif // COMMON_GPU_IFACE_H_
