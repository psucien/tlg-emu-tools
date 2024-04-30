#include <common/ipc_manager.h>

#include <functional>


//------------------------------------------------------------------------------
void
ipc_manager::init()
{
    shmem_ib_handle_ = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,                      // size hi
        SHMEM_INFO_BLOCK_SIZE,  // size lo
        SHMEM_INFO_BLOCK_NAME
    );

    assert(shmem_ib_handle_ != INVALID_HANDLE_VALUE);

    auto *shmem_ptr = MapViewOfFileEx(
        shmem_ib_handle_,
        FILE_MAP_ALL_ACCESS,
        0,  // offs hi
        0,  // offs lo
        SHMEM_INFO_BLOCK_SIZE,
        0
    );

    assert(shmem_ptr != nullptr);

    iblock_addr =
        reinterpret_cast<shmem_info_block_header*>(shmem_ptr);

    // initialize to defaults
    iblock_addr->magic = IBLOCK_MAGIC;
    std::memset(
        iblock_addr->title_name,
        0,
        sizeof(iblock_addr->title_name)
    );

    alloc_shared_stack();
}

//------------------------------------------------------------------------------
void
ipc_manager::destroy()
{
    if (iblock_addr) {
        UnmapViewOfFile(
            reinterpret_cast<void*>(iblock_addr->mem_descs[0].start)
        );
        CloseHandle(shmem_stack_handle_);
    }

    UnmapViewOfFile(iblock_addr);
    CloseHandle(shmem_ib_handle_);
}

//------------------------------------------------------------------------------
void
ipc_manager::alloc_shared_stack()
{
    auto const total_stack_size =
        SHMEM_STACK_SIZE + SHMEM_STACK_PAYLOAD_SIZE;

    shmem_stack_handle_ = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,                  // size hi
        total_stack_size,   // size lo
        SHMEM_REGN_NAME_PREFIX"0"
    );

    assert(shmem_stack_handle_ != INVALID_HANDLE_VALUE);

    auto *shmem_ptr = MapViewOfFileEx(
        shmem_stack_handle_,
        FILE_MAP_ALL_ACCESS,
        0,  // offs hi
        0,  // offs lo
        total_stack_size,
        0
    );

    assert(shmem_ptr != nullptr);
    assert(iblock_addr);
    assert(iblock_addr->magic == IBLOCK_MAGIC);
    assert(iblock_addr->num_mem_regions == 0); // should be very first region

    iblock_addr->mem_descs[0].start = uintptr_t(shmem_ptr);
    iblock_addr->mem_descs[0].size  = total_stack_size;
    ++iblock_addr->num_mem_regions;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_connect()
{
    client_pipe_in_h_ = CreateFileA(
        CMD_PIPE_NAME_OUT,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (client_pipe_in_h_ == INVALID_HANDLE_VALUE) {
        return false;
    }

    client_pipe_out_h_ = CreateFileA(
        CMD_PIPE_NAME_IN,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (client_pipe_out_h_ == INVALID_HANDLE_VALUE) {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_read_packet
        ( ipc_packet   *pkt
        )
{
    DWORD num_bytes_read{ 0 };

    bool result = ReadFile(
        client_pipe_in_h_,
        pkt,
        sizeof(ipc_packet),
        &num_bytes_read,
        NULL
    );

    return result;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_write_packet
        ( ipc_packet   *pkt
        )
{
    DWORD num_bytes_read{ 0 };

    bool result = WriteFile(
        client_pipe_out_h_,
        pkt,
        sizeof(ipc_packet),
        &num_bytes_read,
        NULL
    );

    return result;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_peek_packet
        ( ipc_packet   *pkt
        )
{
    DWORD num_bytes_read{ 0 };

    bool result = PeekNamedPipe(
        client_pipe_in_h_,
        pkt,
        sizeof(ipc_packet),
        &num_bytes_read,
        NULL,
        NULL
    );

    return result;
}

//------------------------------------------------------------------------------
void
ipc_manager::client_disconnect()
{
    CloseHandle(client_pipe_in_h_);
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_start()
{
    server_pipe_in_h_ = CreateNamedPipeA(
        CMD_PIPE_NAME_IN,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE,
        1,
        0,
        0,
        0,
        NULL
    );

    server_pipe_out_h_ = CreateNamedPipeA(
        CMD_PIPE_NAME_OUT,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE,
        1,
        0,
        0,
        0,
        NULL
    );

    thread_ = std::jthread{ std::bind_front(&ipc_manager::wait_for_connection, this) };

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_send_packet
        ( ipc_packet   *pkt
        )
{
    DWORD num_bytes_written{ 0 };

    bool result = WriteFile(
        server_pipe_out_h_,
        pkt,
        sizeof(ipc_packet),
        &num_bytes_written,
        NULL
    );

    if (!result) {
        std::unique_lock<std::mutex> lock{ m_reconnect_ };

        conn_lost_ = true;
        cv_reconnect_.notify_one();
    }

    is_client_connected_ = result; // reset connection status if write failed

    return result;
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_receive_packet
        ( ipc_packet   *pkt
        )
{
    DWORD num_bytes_read{ 0 };

    bool result = ReadFile(
        server_pipe_in_h_,
        pkt,
        sizeof(ipc_packet),
        &num_bytes_read,
        NULL
    );

    if (!result) {
        std::unique_lock<std::mutex> lock{ m_reconnect_ };

        conn_lost_ = true;
        cv_reconnect_.notify_one();
    }

    is_client_connected_ = result; // reset connection status if read failed

    return result;
}

//------------------------------------------------------------------------------
void
ipc_manager::server_wait_for_client()
{
    ConnectNamedPipe(
        server_pipe_in_h_,
        NULL
    );
}

//------------------------------------------------------------------------------
void
ipc_manager::wait_for_connection
        ( std::stop_token   stoken
        )
{
    while (!stoken.stop_requested()) {
        is_client_connected_ = ConnectNamedPipe(
            server_pipe_out_h_,
            NULL
        );

        if (!is_client_connected_) {
            DisconnectNamedPipe(server_pipe_out_h_);
            continue;
        }

        {
            std::unique_lock<std::mutex> lock{ m_reconnect_ };

            conn_lost_ = false;
            cv_reconnect_.wait(lock, [&]() { return conn_lost_; });
        }
    }
}

//------------------------------------------------------------------------------
void
ipc_manager::server_stop()
{
    // TODO: unlock pipe
    thread_.request_stop();
}
