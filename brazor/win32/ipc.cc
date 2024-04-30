#include "brazor/brazor.h"

#include <functional>


//-----------------------------------------------------------------------------
bool
debugger_module::is_emulator_running()
{
    /* Try to map and read a valid iblock */

    if (!iblock.handle) {
        auto const map_file_h = OpenFileMappingA(
            FILE_MAP_READ | FILE_MAP_WRITE,
            FALSE,
            SHMEM_INFO_BLOCK_NAME
        );

        if (!map_file_h) {
            return false;
        }

        iblock.handle = map_file_h;
    }

    if (iblock.ptr == nullptr) {
        void* shmem_ptr = MapViewOfFile(
            iblock.handle,
            FILE_MAP_READ | FILE_MAP_WRITE,
            0,
            0,
            SHMEM_INFO_BLOCK_SIZE
        );

        if (shmem_ptr == nullptr) {
            return false;
        }

        iblock.ptr = shmem_ptr;
    }

    auto const *iblock_ptr =
        reinterpret_cast<shmem_info_block_header const *>(iblock.ptr);
    if (iblock_ptr->magic != IBLOCK_MAGIC) {
        return false;
    }

    if (iblock_ptr->num_vo_buffers == 0) {
        // TODO: in cases, when VO is not opened (e.g. cs-add sample)
        // or an application starts with compute workload, it won't work.
        // From other side, removing this check may cause a crash on accessing
        // VO buffers which potentially can be not yet initialized.
        return false; // wait until VO is opened
    }

    return true;
}

//-----------------------------------------------------------------------------
bool
debugger_module::connect()
{
    /* Process memory mappings */

    std::fill(
        mem_regions.begin(),
        mem_regions.end(),
        mem_region_desc_t{}
    );

    auto const *iblock_ptr =
        reinterpret_cast<shmem_info_block_header const *>(iblock.ptr);

    for (auto &reg : mem_regions) {
        auto const reg_idx = std::distance(mem_regions.data(), &reg);

        if (reg_idx < iblock_ptr->num_mem_regions) {

            auto const &mmap_name =
                std::format(SHMEM_REGN_NAME_PREFIX"{}", reg_idx);

            reg.handle = OpenFileMappingA(
                FILE_MAP_READ | FILE_MAP_WRITE,
                FALSE,
                mmap_name.c_str()
            );

            if (reg.handle == INVALID_HANDLE_VALUE) {
                disconnect();
                return false;
            }

            reg.ptr = MapViewOfFileEx(
                reg.handle,
                FILE_MAP_READ | FILE_MAP_WRITE,
                0,
                0,
                iblock_ptr->mem_descs[reg_idx].size,
                reinterpret_cast<void*>(iblock_ptr->mem_descs[reg_idx].start)
            );

            if (reg.ptr == nullptr) {
                disconnect();
                return false;
            }
        }
        else {
            reg = mem_region_desc_t{};
        }
    }

    if (!g_ipc_manager.client_connect()) {
        disconnect();
        return false;
    }

    msg_process = std::jthread{ std::bind_front(&debugger_module::thread_func, this) };
    connected = true;

    return true;
}

//-----------------------------------------------------------------------------
void
debugger_module::disconnect()
{
    if (connected) {
        msg_process.request_stop();
        cv_pause.notify_all();
    }

    g_ipc_manager.client_disconnect();

    for (auto &reg : mem_regions) {
        if (reg.ptr != nullptr) {
            UnmapViewOfFile(reg.ptr);
        }
        reg.handle.~smart_handle();

        reg = mem_region_desc_t{};
    }

    {
        UnmapViewOfFile(iblock.ptr);
        iblock.handle.~smart_handle();

        iblock = mem_region_desc_t{};
    }

    connected = false;
    online_mode = false;
}
