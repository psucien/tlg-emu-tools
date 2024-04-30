#include <common/ipc_manager.h>


//------------------------------------------------------------------------------
void
ipc_manager::init()
{
    // TBI

    alloc_shared_stack();
}

//------------------------------------------------------------------------------
void
ipc_manager::destroy()
{
    // TBI
}

//------------------------------------------------------------------------------
void
ipc_manager::alloc_shared_stack()
{
    auto const total_stack_size =
        SHMEM_STACK_SIZE + SHMEM_STACK_PAYLOAD_SIZE;

    // TBI
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_connect()
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_read_packet
        ( ipc_packet   *pkt
        )
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_write_packet
        ( ipc_packet   *pkt
        )
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::client_peek_packet
        ( ipc_packet   *pkt
        )
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
void
ipc_manager::client_disconnect()
{
    // TBI
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_start()
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_send_packet
        ( ipc_packet   *pkt
        )
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
bool
ipc_manager::server_receive_packet
        ( ipc_packet   *pkt
        )
{
    // TBI

    return false;
}

//------------------------------------------------------------------------------
void
ipc_manager::server_wait_for_client()
{
    // TBI
}

//------------------------------------------------------------------------------
void
ipc_manager::wait_for_connection
        ( std::stop_token   stoken
        )
{
    while (!stoken.stop_requested()) {

        // TBI

    }
}

//------------------------------------------------------------------------------
void
ipc_manager::server_stop()
{
    // TODO: send signal
    thread_.join();
}
