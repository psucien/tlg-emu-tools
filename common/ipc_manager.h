#ifndef COMMON_IPC_MANAGER_H_
#define COMMON_IPC_MANAGER_H_

#include "common.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <condition_variable>
#include <cstdint>
#include <thread>


#define SHMEM_INFO_BLOCK_NAME       "emu-iblock"
#define SHMEM_INFO_BLOCK_SIZE       (16 * 1024u)
#define SHMEM_REGN_NAME_PREFIX      "emu-mreg-"
#define SHMEM_STACK_SIZE            (15 * 1024 * 1024u) // 15x more than Windows does. TODO: consider a pool of stacks for all threads spawned by game
#define SHMEM_STACK_PAYLOAD_SIZE    (1024u)

#define IBLOCK_MAGIC                (0x55AA3311)
#define MAX_MEMORY_REGIONS          (8u)

#define MAX_VO_BUFFERS              (3u) // triple buffering

#define CMD_PIPE_NAME_IN            "\\\\.\\pipe\\gpu-iface-cmd-in"
#define CMD_PIPE_NAME_OUT           "\\\\.\\pipe\\gpu-iface-cmd-out"

using vo_buffer_desc_t = struct
{
    uintptr_t           addr;
    uint32_t            fmt;
    uint32_t            pitch;
    uint32_t            width          : 16;
    uint32_t            height         : 16;
    uint32_t            tiling_mode    : 1;
    uint32_t            aspect         : 1;
};

using stack_payload_t = struct
{
    // payload should go first, because it is used as temporary storage for embedded VS
    // The address of VS is 256 bytes aligned
    uint8_t             payload[1024 - MAX_VO_BUFFERS * 8];
    uint64_t            vo_labels[MAX_VO_BUFFERS];
};
static_assert(sizeof(stack_payload_t) == SHMEM_STACK_PAYLOAD_SIZE);

struct shmem_info_block_header
{
    static auto constexpr MAX_TITLE_NAME = 255ul;

    uint32_t                        magic{ 0 };
    uint32_t                        num_mem_regions{ 0 };
    memory_region_desc              mem_descs[MAX_MEMORY_REGIONS];
    vo_buffer_desc_t                vo_buffers[MAX_VO_BUFFERS];
    uint32_t                        num_vo_buffers{ 0 };
    uint64_t                        num_frames_submitted{ 0 };
    char                            title_name[MAX_TITLE_NAME];
};
static_assert(sizeof(shmem_info_block_header) <= SHMEM_INFO_BLOCK_SIZE);

#define MAX_DCB_CCB_PAIRS   (1u) // USECASE: haven't seen more

// We need to store actual values of ptrs and sizes, because memory holding them
// at submission moment may be re-used while packet is in flight
struct gfx_packet
{
    uint32_t    count;
    void       *dcb_gpu_addrs[MAX_DCB_CCB_PAIRS];
    uint32_t    dcb_sizes_bytes[MAX_DCB_CCB_PAIRS];
    void       *ccb_gpu_addrs[MAX_DCB_CCB_PAIRS];
    uint32_t    ccb_sizes_bytes[MAX_DCB_CCB_PAIRS];
};

struct asc_packet
{
    uint32_t    vqid;
    uint32_t    size;
    void       *cmdb;
};

struct flip_packet
{
    uint32_t    handle;
    uint32_t    index;
    uint32_t    mode;
    uint32_t    arg;
    bool        eop;
};

struct irq_packet
{
    uint32_t    event;
};

struct ipc_packet
{
    enum class opcode_t : uint32_t
    {
        eGFX_SUBMIT,
        eASC_SUBMIT,
        eFLIP_REQUEST,
        eSUBMIT_DONE,
        eIRQ,
        eGPU_IDLE,
        eCLOSE_CONNECTION,
    } opcode;

    union
    {
        gfx_packet  gfx;
        asc_packet  asc;
        flip_packet flip;
        irq_packet  irq;
    };
};

struct ipc_manager
{
    void init();
    void destroy();

    void alloc_shared_stack();

    bool client_connect();
    bool client_read_packet(ipc_packet *pkt);
    bool client_write_packet(ipc_packet *pkt);
    bool client_peek_packet(ipc_packet *pkt);
    void client_disconnect();

    bool server_start();
    bool server_send_packet(ipc_packet *pkt);
    bool server_receive_packet(ipc_packet *pkt);
    bool is_client_connected() const
    {
        return is_client_connected_;
    }
    void server_stop();
    void server_wait_for_client();

    shmem_info_block_header *iblock_addr{ nullptr };
private:
    void wait_for_connection();

    std::thread thread_{};
#ifdef _WIN32
    HANDLE shmem_ib_handle_{ INVALID_HANDLE_VALUE };
    HANDLE shmem_stack_handle_{ INVALID_HANDLE_VALUE };
    HANDLE client_pipe_in_h_{ INVALID_HANDLE_VALUE };
    HANDLE client_pipe_out_h_{ INVALID_HANDLE_VALUE };
    HANDLE server_pipe_in_h_{ INVALID_HANDLE_VALUE };
    HANDLE server_pipe_out_h_{ INVALID_HANDLE_VALUE };
#endif
    bool is_client_connected_{ false };

    std::condition_variable cv_reconnect_{};
    std::mutex              m_reconnect_{};
    bool                    conn_lost_{ false };
};

extern ipc_manager g_ipc_manager;

#endif // COMMON_IPC_MANAGER_H_
