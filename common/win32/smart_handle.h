#ifndef COMMON_WIN32_SMART_HANDLE_H__
#define COMMON_WIN32_SMART_HANDLE_H__

#include <windows.h>

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

#endif // COMMON_WIN32_SMART_HANDLE_H__
