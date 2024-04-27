#include "brazor.h"
#include "config.h"
#include "gui.h"

#include <common/ipc_manager.h>

#define SOKOL_IMPL
#if _WIN32
#   define SOKOL_D3D11
#else
#   define SOKOL_GLCORE33
#endif
#include <sokol/sokol_app.h>
#include <sokol/sokol_gfx.h>
#include <sokol/sokol_glue.h>
#include <sokol/sokol_time.h>

#include <imgui/imgui.h>
#include <sokol/util/sokol_imgui.h>

#include <fstream>

debugger_module tool{};

// Tmp placeholders {
bool
debugger_module::is_emulator_running()
{
    return false;
}

bool
debugger_module::connect()
{
    return false;
}

std::unique_ptr<uint8_t[]> cmdlist_data{}; // to make Turtle happy ;)
size_t cmdlist_sz{};

void
sokol_logger
        ( char const   *tag                // always "sapp"
        , uint32_t      log_level          // 0=panic, 1=error, 2=warning, 3=info
        , uint32_t      log_item_id        // SAPP_LOGITEM_*
        , char const   *message_or_null    // a message string, may be nullptr in release mode
        , uint32_t      line_nr            // line number in sokol_app.h
        , char const   *filename_or_null   // source filename, may be nullptr in release mode
        , void         *user_data
        )
{
#if !defined(_WIN32)
    printf(
        "%d:[%3d] %s (%s:%d)\n",
        log_level,
        log_item_id,
        message_or_null,
        filename_or_null,
        line_nr
    );
#endif
}
// }

//----------------------------------------------------------------------------
// https://github.com/ocornut/imgui/discussions/3862
bool
aligned_button
        ( char const   *label
        , float         alignment /* = 0.5f*/
        )
{
    auto& style = ImGui::GetStyle();

    auto const size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    auto const avail = ImGui::GetContentRegionAvail().x;

    auto const off = (avail - size) * alignment;
    if (off > 0.0f) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

    return ImGui::Button(label);
}

//----------------------------------------------------------------------------
static inline sg_pixel_format
vo_fmt_convert
        ( uint32_t fmt
        )
{
#if 0 // TODO: ipc
    if (fmt == SCE_VIDEO_OUT_PIXEL_FORMAT_A8R8G8B8_SRGB) {
        return SG_PIXELFORMAT_BGRA8;
    }
    assert(0);
#endif
    return SG_PIXELFORMAT_NONE;
}

//----------------------------------------------------------------------------
static inline size_t
vo_surface_size
        ( int buf_idx
        )
{
    auto const *iblock_ptr =
        reinterpret_cast<shmem_info_block_header const *>(tool.iblock.ptr);

#if 0 // TODO: ipc
    auto const &vo_surface = iblock_ptr->vo_buffers[buf_idx];
    switch (vo_surface.fmt) {
        case SCE_VIDEO_OUT_PIXEL_FORMAT_A8R8G8B8_SRGB:
            [[fallthrough]];
        case SCE_VIDEO_OUT_PIXEL_FORMAT_A8B8G8R8_SRGB: {
            return vo_surface.pitch * vo_surface.height * 4;
        }
        default: {
            assert(0);
        }
    }
#endif
    return 0;
}

struct
{
    sg_image    src_img;
    sg_shader   sh;
    sg_pipeline pl;
    sg_sampler  smp;
    sg_buffer   vbuf;
    sg_bindings bind;
    std::mutex  m_flip{};
    bool        do_flip{ false };
    int         buf_idx;
} blit{};

struct
{
    uint64_t        laptime;
    sg_pass_action  pass_action;
} state{};

//----------------------------------------------------------------------------
static void
flip_handler()
{
#if 0 // TODO: ipc
    if (gpu_.flip_req.empty()) {
        return;
    }

    auto const buf_idx = gpu_.flip_req.front().index;
    tool.flip(buf_idx);
    gpu_.flip_req.pop();
#endif
}

//----------------------------------------------------------------------------
static void
irq_handler
        ( int event_id
        )
{
#if 0 // TODO: ipc
    ipc_packet pkt{};
    pkt.opcode    = ipc_packet::opcode_t::eIRQ;
    pkt.irq.event = event_id;
    g_ipc_manager.client_write_packet(&pkt);
#endif
}

//----------------------------------------------------------------------------
static void
idle_handler()
{
#if 0 // TODO: ipc
    ipc_packet pkt{};
    pkt.opcode    = ipc_packet::opcode_t::eGPU_IDLE;
    g_ipc_manager.client_write_packet(&pkt);
#endif
}

//----------------------------------------------------------------------------
void
debugger_module::flip
    ( int buf_idx
    )
{
    std::unique_lock<std::mutex> lock{ blit.m_flip };

    auto const *iblock_ptr =
        reinterpret_cast<shmem_info_block_header const *>(tool.iblock.ptr);

    auto const payload_addr = iblock_ptr->mem_descs[0].start + SHMEM_STACK_SIZE;
    auto* vo_labels = reinterpret_cast<stack_payload_t*>(payload_addr)->vo_labels;

    if (do_render) {
        // There is a chance, that the next frame will be ready faster than
        // GUI logic will do actual present. In this case, the resource will
        // be updated twice and this is forbidden by Sokol. We need to postpone
        // uploading till the next frame().
        blit.do_flip = true;
        blit.buf_idx = buf_idx;
    }

    //frame_end_t[0] =
    //    std::chrono::high_resolution_clock::now();
    //frame_time = std::chrono::duration_cast<std::chrono::microseconds>(
    //    tool.frame_end_t[0] - tool.frame_start_t[0]
    //).count();

    vo_labels[buf_idx] = 0;

    irq_handler(255);
}

//----------------------------------------------------------------------------
void
init()
{
    stm_setup();

    sg_desc desc{
        .environment = sglue_environment(),
    };
    sg_setup(&desc);

    simgui_desc_t imgui_desc{ 0 };
    simgui_setup(&imgui_desc);

    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    state.pass_action.colors[0].clear_value = { 0.63f, 0.63f, 0.63f, 1.0f };

    auto& style = ImGui::GetStyle();
    style.WindowRounding = 5.3f;

    // A vertex buffer with the triangle vertices.
    // Unfortunatelly, we can't do rendering in Sokol without VB provided
    const float vertices[] = {
        // positions            uv
        -1.0f, -1.0f, 0.0f,     0.0f,  1.0f,
        -1.0f,  3.0f, 0.0f,     0.0f, -1.0f,
         3.0f, -1.0f, 0.0f,     2.0f,  1.0f,
    };

    sg_buffer_desc vbuf_desc{};
    vbuf_desc.data = SG_RANGE(vertices);
    blit.vbuf = sg_make_buffer(&vbuf_desc);

    sg_sampler_desc smp_desc{};
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_LINEAR;
    blit.smp = sg_make_sampler(&smp_desc);

    blit.bind.vertex_buffers[0] = blit.vbuf;
    blit.bind.fs.samplers[0]    = blit.smp;

#if _WIN32
    sg_shader_desc sh_desc{};
    sh_desc.attrs[0].sem_name = "POSITION";
    sh_desc.attrs[1].sem_name = "TEXCOORD";
    sh_desc.attrs[1].sem_index = 1;
    sh_desc.vs.source =
        "struct vs_in {\n"
        "  float4 pos: POSITION;\n"
        "  float2  uv: TEXCOORD1;\n"
        "};\n"
        "struct vs_out {\n"
        "  float2  uv: TEXCOORD0;\n"
        "  float4 pos: SV_Position;\n"
        "};\n"
        "vs_out main(vs_in inp) {\n"
        "  vs_out outp;\n"
        "  outp.pos = inp.pos;\n"
        "  outp.uv  = inp.uv;\n"
        "  return outp;\n"
        "}\n";
    sh_desc.fs.source =
        "Texture2D<float4> tex: register(t0);\n"
        "sampler smp: register(s0);\n"
        "float4 main(float2 uv: TEXCOORD0): SV_Target0 {\n"
        "  return tex.Sample(smp, uv);\n"
        "}\n";
    sh_desc.fs.images[0].used = true;
    sh_desc.fs.samplers[0].used = true;
    sh_desc.fs.image_sampler_pairs[0].used = true;

    blit.sh = sg_make_shader(&sh_desc);

    sg_pipeline_desc pl_desc{};
    pl_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
    pl_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2;
    pl_desc.shader = blit.sh;
    blit.pl = sg_make_pipeline(&pl_desc);
#endif

#if 0
    gpu_.init();
    gpu_.cb_gpu_idle       = idle_handler;
    gpu_.gfx_q->cp.cb_intr = irq_handler;
    gpu_.gfx_q->cp.cb_flip = flip_handler;
#endif
}

//----------------------------------------------------------------------------
void
capture_mode
        ( double    delta_time
        )
{
    if (!tool.cap_is_opened) {
        return;
    }

#if 0
    render_ovl_status();
    render_dbg_cmdlist();
    if (gpu_.gfx_q->cp.is_bp_reached
        || gpu_.gfx_q->cp.dbg_batch_start != 0) {
        render_dbg_pipeline();
    }
    render_dbg_wavefront();
#endif
}

//----------------------------------------------------------------------------
void
online_mode
        ( double    delta_time
        , bool      has_connection
        )
{
    if (!has_connection) {
        auto const window_flags =
            ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoNav;

        // Center window
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2{ 0.5f, 0.5f });
        ImGui::SetNextWindowBgAlpha(0.35f);

        bool p_open{};
        if (ImGui::Begin("Overlay:connection", &p_open, window_flags)) {
            if (!tool.is_emulator_running()) {
                static auto throbber_time{ 0.0 };
                static const auto delta_wait = 0.73;
                static char const* throbber[] = {
                    ".  ",
                    ".. ",
                    "...",
                    " ..",
                    "  .",
                    "   "
                };
                static int throbber_idx{ 0 };

                throbber_time += delta_time;
                if (throbber_time > delta_wait) {
                    throbber_idx = (throbber_idx + 1) % (ARRAY_SIZE(throbber) - 1);
                    throbber_time = 0.0;
                }

                ImGui::NewLine();
                ImGui::Text(" Waiting for connection");
                ImGui::SameLine();
                ImGui::Text("%s", throbber[throbber_idx]);
                ImGui::NewLine();
            }
            else {
                if (tool.connect()) {
                    auto const *iblock_ptr =
                        reinterpret_cast<shmem_info_block_header const*>(tool.iblock.ptr);
                    std::string app_title = std::string{ APP_NAME }.append(" : ")
                        + iblock_ptr->title_name;
                    sapp_set_window_title(app_title.c_str());
                }
            }
            ImGui::End();
        }
    }
    else {
        //render_ovl_status();
    }
}

//----------------------------------------------------------------------------
void
frame()
{
    // critical section to flip VO surface
    {
        std::unique_lock<std::mutex> lock{ blit.m_flip };

        if (blit.do_flip) {
            auto const *iblock_ptr =
                reinterpret_cast<shmem_info_block_header const *>(tool.iblock.ptr);

            //auto surface =
            //    utils::detile_rt(iblock_ptr->vo_buffers[blit.buf_idx].addr);

            sg_image_data img_data{};
            img_data.subimage[0][0] = sg_range{
                reinterpret_cast<void*>(
                    iblock_ptr->vo_buffers[blit.buf_idx].addr
                ),
                vo_surface_size(blit.buf_idx)
            };
            sg_update_image(
                blit.src_img,
                img_data
            );

            blit.do_flip = false;
        }
    }

    auto const has_connection = tool.is_connected(); // cache once to have consistent value for the frame

    const int width = sapp_width();
    const int height = sapp_height();
    const double delta_time = stm_sec(stm_round_to_common_refresh_rate(stm_laptime(&state.laptime)));
    simgui_frame_desc_t desc;
    desc.delta_time = delta_time;
    desc.width = width;
    desc.height = height;
    desc.dpi_scale = sapp_dpi_scale();

    simgui_new_frame(desc);

    /*=== UI CODE STARTS HERE ===*/
    //render_main_menu();

    if (tool.online_mode) {
        online_mode(delta_time, has_connection);
    }
    else {
        capture_mode(delta_time);
    }

    //render_captures_list();
    //render_capture_status();

    if (cmdlist_data) {
        extern void render_dbg_cmdlist(uintptr_t, size_t);
        render_dbg_cmdlist(
            uintptr_t(cmdlist_data.get()),
            cmdlist_sz
        );
    }

    /*=== UI CODE ENDS HERE ===*/

    sg_pass pass_desc{
        .action     = state.pass_action,
        .swapchain  = sglue_swapchain()
    };
    sg_begin_pass(&pass_desc);

    if (has_connection || tool.cap_is_opened) {
        auto const *iblock_ptr =
            reinterpret_cast<shmem_info_block_header const *>(tool.iblock.ptr);

        // initialize VO surface
        if (blit.src_img.id == SG_INVALID_ID) {
            auto const &vo_buffer = iblock_ptr->vo_buffers[0];
            sg_image_desc desc{};
            desc.width          = vo_buffer.width;
            desc.height         = vo_buffer.height;
            desc.type           = SG_IMAGETYPE_2D;
            desc.usage          = SG_USAGE_DYNAMIC;
            desc.pixel_format   = vo_fmt_convert(vo_buffer.fmt);
            blit.src_img = sg_make_image(&desc);

            blit.bind.fs.images[0] = blit.src_img;
        }

        sg_apply_pipeline(blit.pl);
        sg_apply_bindings(&blit.bind);
        sg_draw(0, 3, 1);
    }

    simgui_render();
    sg_end_pass();
    sg_commit();
}

//----------------------------------------------------------------------------
void
on_event
    ( sapp_event const *ev
    )
{
    if (ev->type == SAPP_EVENTTYPE_FILES_DROPPED) {
        auto const num_dropped_files = sapp_get_num_dropped_files();

        if (num_dropped_files > 0) {
            assert(num_dropped_files == 1);
            auto const* path = sapp_get_dropped_file_path(0);
            std::ifstream fin{ path, std::ofstream::in | std::ofstream::ate | std::ofstream::binary };
            if (fin.is_open()) {
                size_t const f_sz = fin.tellg();
                fin.seekg(0);

                cmdlist_data = std::make_unique<uint8_t[]>(f_sz);
                cmdlist_sz = f_sz;

                fin.read(
                    reinterpret_cast<char*>(cmdlist_data.get()),
                    f_sz
                );
                fin.close();
            }
        }
    }

    simgui_handle_event(ev);
}

//----------------------------------------------------------------------------
void
cleanup()
{
    simgui_shutdown();
    sg_shutdown();
    //gpu().destroy();
}

//----------------------------------------------------------------------------
sapp_desc
sokol_main
    ( int   argc
    , char *argv[]
    )
{
    sapp_desc desc{};
    desc.init_cb            = init;
    desc.frame_cb           = frame;
    desc.cleanup_cb         = cleanup;
    desc.event_cb           = on_event;
    desc.width              = WINDOW_DIM_X;
    desc.height             = WINDOW_DIM_Y;
    desc.window_title       = APP_NAME;
    desc.enable_dragndrop   = true;
    desc.logger             = { sokol_logger, nullptr };
    return desc;
}
