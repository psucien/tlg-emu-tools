#ifndef GPU_SHARED_H_
#define GPU_SHARED_H_

#include <common/display_iface.h>
#include <common/gpu_iface.h>

#include <array>

static auto constexpr MAX_GRAPHICS_BACKENDS = 4u;

static auto constexpr EV_FLIP = 0xffu;

static auto constexpr GFX_DCB_TOKEN = 0xffu;
static auto constexpr GFX_cCB_TOKEN = 0xfeu;

struct graphics_backend_t
{
    graphics_backend_t(
        void* _vo,
        void* _gpu);

    i_display   *vo;
    i_gpu       *gpu;
};

extern uint32_t g_num_backends;
extern uint32_t g_backend_selected;
extern std::array <graphics_backend_t*, MAX_GRAPHICS_BACKENDS> g_backends;

i_gpu* gpu();
i_display* display();

#endif // GPU_SHARED_H_
