#ifndef COMMON_CMDPROC_IFACE_H_
#define COMMON_CMDPROC_IFACE_H_

#include <cstdint>

/*
 * Generic PM4 header
 */
typedef union PM4_HEADER
{
    struct
    {
        uint32_t    reserved    : 16;
        uint32_t    count       : 14;
        uint32_t    type        :  2; // PM4_TYPE
    };
    uint32_t u32All;

} PM4_HEADER, *PPM4_HEADER;


struct i_cmdproc
{
    template <typename HdrType>
    static inline HdrType
    get_next
        ( HdrType   this_pm4
        , uint32_t  n
        )
    {
        HdrType curr_pm4 = this_pm4;
        while (n) {
            curr_pm4 = reinterpret_cast<HdrType>(
                reinterpret_cast<uint32_t const*>(curr_pm4) + curr_pm4->count + 2
            );
            --n;
        }
        return curr_pm4;
    }

    enum class result_t
    {
        eDone,
        eCall,
        eYield,
        eBreak
    };

    virtual result_t exec_cmdb(uintptr_t cmdb_addr, size_t cmdb_size) = 0;
};

#endif // COMMON_CMDPROC_IFACE_H_
