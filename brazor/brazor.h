#ifndef BRAZOR_H_
#define BRAZOR_H_

#include <common/ipc_manager.h>

#include <array>
#include <chrono>
#include <vector>

#if _WIN32
#   include <common/win32/smart_handle.h>
#else
using smart_handle = uint32_t;
#endif

struct debugger_module
{
    bool is_emulator_running();
    bool is_connected() const { return connected; };

    bool connect();
    void disconnect();

    void dump_capture(std::string const &name);
    void dump_capture_compressed(std::string const &name);
    void load_capture(std::string const &name);
    void replay_capture();
    void close_capture();

    void set_cmdb_bp(int qid, uintptr_t addr);
    void set_spi_bp(
        int qid,
        uintptr_t addr,
        int shader_stage,
        int wg,
        int tid);

    void thread_func(std::stop_token stoken);

    void flip(int buf_idx);

    struct mem_region_desc_t {
        smart_handle    handle{};
        void           *ptr{nullptr};
    };
    mem_region_desc_t                                   iblock{};
    std::array<mem_region_desc_t, MAX_MEMORY_REGIONS>   mem_regions{};
    bool                                                connected{ false };

    std::jthread                msg_process{};

    bool                        online_mode{ false };

    //std::unique_ptr<capture>    capture_{};

    std::mutex                  m_pause{};
    std::condition_variable     cv_pause{};
    bool                        do_pause{ true };
    bool                        do_render{ false };

#if 0
    struct mem_file_data
    {
        HANDLE          h_mem_file;
        HANDLE          h_mapping;
        void           *vmem_ptr;
    };

    std::vector<mem_file_data>              mem_files{};
#endif

    bool                                    cap_is_opened{ false };

#if _WIN64
    using time_point = std::chrono::steady_clock::time_point;
#else
    using time_point = std::chrono::system_clock::time_point;
#endif
    std::array<time_point, MAX_VO_BUFFERS>  frame_start_t{};
    std::array<time_point, MAX_VO_BUFFERS>  frame_end_t{};
    size_t                                  frame_time{};
};

extern debugger_module tool;

#endif // BRAZOR_H_
