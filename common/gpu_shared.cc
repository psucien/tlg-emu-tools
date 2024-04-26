#include "gpu_shared.h"

#include <cassert>


extern graphics_backend_t null;
extern graphics_backend_t ipc;
extern graphics_backend_t sw;
extern graphics_backend_t vk;

uint32_t g_num_backends{ 0 };
uint32_t g_backend_selected{ 0 };
std::array <graphics_backend_t*, MAX_GRAPHICS_BACKENDS> g_backends{
    //&null,
    //&ipc,
    //&vk,
    //&sw
};

graphics_backend_t::graphics_backend_t
        ( void *_vo
        , void *_gpu
        )
{
    vo  = reinterpret_cast<i_display *>(_vo);
    gpu = reinterpret_cast<i_gpu *>(_gpu);
    ++g_num_backends;
}

bool g_show_splash = true;

//----------------------------------------------------------------------------
i_gpu*
gpu()
{
    assert(g_num_backends > g_backend_selected);
    return g_backends[g_backend_selected]->gpu;
}

//----------------------------------------------------------------------------
i_display*
display()
{
    assert(g_num_backends > g_backend_selected);
    return g_backends[g_backend_selected]->vo;
}
