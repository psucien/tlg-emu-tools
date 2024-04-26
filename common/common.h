#ifndef COMMON_H_
#define COMMON_H_

#include <cstdio>
#include <cstdint>
#include <cassert>


struct memory_region_desc
{
    uintptr_t   start;
    size_t      size;
    uint32_t    protection;
    uint32_t    flags;
};

using rect_t = struct
{
    uint32_t    left;
    uint32_t    top;
    uint32_t    right;
    uint32_t    bottom;
};

template<typename T>
union point2_t
{
    struct
    {
        T   x;
        T   y;
    };
    T c[2];
};

template<typename T>
union point3_t
{
    struct
    {
        T   x;
        T   y;
        T   z;
    };
    T c[3];
};

template<typename T>
union point4_t
{
    point4_t() {}
    struct
    {
        T   x;
        T   y;
        T   z;
        T   w;
    };
    T c[4];
};

template<typename T>
using vec2 = point2_t<T>;

template<typename T>
using vec3 = point3_t<T>;

template<typename T>
using vec4 = point4_t<T>;

#define ROUND_UP(x, b)      (((x) + (b) - 1) / (b))
#define ROUND_DOWN(x, b)    (((x) / (b)) * (b))
#define ARRAY_SIZE(x)       (sizeof(x) / sizeof((x)[0]))

#define PS4API          __attribute__((sysv_abi))
#define PS4NORETURN     __attribute__((noreturn))
#define PS4NAKED        __attribute__((naked))
#define PS4NOINLINE     __attribute__((noinline))
#define PS4DEPRECATED   __attribute__((deprecated))
#define PS4UNUSED       __attribute__((unused))
#define PS4ALIGN(n)     __attribute__((__aligned__(n)))

#define LOG_DEBUG(format, ...)                          do { printf("D|" format "\n", __VA_ARGS__); } while (0)
#define LOG_TRACE(format, ...)                          do { printf("T|" format "\n", __VA_ARGS__); } while (0)
#define LOG_FIXME(format, ...)                          do { printf("F|(%s)" format "\n", __FUNCTION__, __VA_ARGS__); } while (0)
#define LOG_WARN(format, ...)                           do { printf("W|" format "\n", __VA_ARGS__); } while (0)
#define LOG_ERR(format, ...)                            do { printf("E|" format "\n", __VA_ARGS__); } while (0)
#define LOG_ASSERT(expr, format, ...)                   do { if (!(expr)) { printf(format"\n", __VA_ARGS__); assert(0); } } while (0)
#define LOG_ASSERT_RETURN(expr, retval, format, ...)

#endif
