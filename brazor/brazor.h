#ifndef BRAZOR_H_
#define BRAZOR_H_

#include <common/ipc_manager.h>

#include <windows.h>

#include <array>
#include <chrono>
#include <vector>


struct smart_handle
{
    smart_handle() = default;
    explicit smart_handle(HANDLE h) : storage(h) {}
    ~smart_handle()
    {
        CloseHandle(storage);
        storage = INVALID_HANDLE_VALUE;
    }

    HANDLE operator=(HANDLE in)
    {
        return storage = in;
    }

    operator HANDLE() const
    {
        return storage;
    }

    bool operator!() const
    {
        return (storage == INVALID_HANDLE_VALUE)
            || (storage == 0);
    }

    HANDLE storage{ INVALID_HANDLE_VALUE };
};


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

    struct mem_file_data
    {
        HANDLE          h_mem_file;
        HANDLE          h_mapping;
        void           *vmem_ptr;
    };

    std::vector<mem_file_data>              mem_files{};

    bool                                    cap_is_opened{ false };

    std::chrono::steady_clock::time_point   frame_start_t[MAX_VO_BUFFERS];
    std::chrono::steady_clock::time_point   frame_end_t[MAX_VO_BUFFERS];
    size_t                                  frame_time{};
};

extern debugger_module tool;

#endif // BRAZOR_H_
