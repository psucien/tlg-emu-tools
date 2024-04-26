#include "gcn/si_ci_vi_merged_offset.h"

#include <cstdint>

using namespace Pal::Gfx6;

char const *
get_shader_reg_name
        ( uint32_t reg_offset
        )
{
    switch (reg_offset) {
        case mmSPI_SHADER_PGM_LO_VS:
            return"mmSPI_SHADER_PGM_LO_VS";
        case mmSPI_SHADER_PGM_HI_VS:
            return"mmSPI_SHADER_PGM_HI_VS";
        case mmSPI_SHADER_PGM_LO_PS:
            return"mmSPI_SHADER_PGM_LO_PS";
        case mmSPI_SHADER_PGM_HI_PS:
            return"mmSPI_SHADER_PGM_HI_PS";
        case mmSPI_SHADER_PGM_RSRC1_VS:
            return"mmSPI_SHADER_PGM_RSRC1_VS";
        case mmSPI_SHADER_PGM_RSRC2_VS:
            return"mmSPI_SHADER_PGM_RSRC2_VS";
        case mmSPI_SHADER_PGM_RSRC3_VS__CI__VI:
            return"mmSPI_SHADER_PGM_RSRC3_VS__CI__VI";
        case mmSPI_SHADER_PGM_RSRC1_PS:
            return"mmSPI_SHADER_PGM_RSRC1_PS";
        case mmSPI_SHADER_PGM_RSRC2_PS:
            return"mmSPI_SHADER_PGM_RSRC2_PS";
        case mmSPI_SHADER_PGM_RSRC3_PS__CI__VI:
            return"mmSPI_SHADER_PGM_RSRC3_PS__CI__VI";
        case mmSPI_SHADER_USER_DATA_PS_0:
            return"mmSPI_SHADER_USER_DATA_PS_0";
        case mmSPI_SHADER_USER_DATA_PS_1:
            return"mmSPI_SHADER_USER_DATA_PS_1";
        case mmSPI_SHADER_USER_DATA_PS_2:
            return"mmSPI_SHADER_USER_DATA_PS_2";
        case mmSPI_SHADER_USER_DATA_PS_3:
            return"mmSPI_SHADER_USER_DATA_PS_3";
        case mmSPI_SHADER_USER_DATA_PS_4:
            return"mmSPI_SHADER_USER_DATA_PS_4";
        case mmSPI_SHADER_USER_DATA_PS_5:
            return"mmSPI_SHADER_USER_DATA_PS_5";
        case mmSPI_SHADER_USER_DATA_PS_6:
            return"mmSPI_SHADER_USER_DATA_PS_6";
        case mmSPI_SHADER_USER_DATA_PS_7:
            return"mmSPI_SHADER_USER_DATA_PS_7";
        case mmSPI_SHADER_USER_DATA_PS_8:
            return"mmSPI_SHADER_USER_DATA_PS_8";
        case mmSPI_SHADER_USER_DATA_PS_9:
            return"mmSPI_SHADER_USER_DATA_PS_9";
        case mmSPI_SHADER_USER_DATA_PS_10:
            return"mmSPI_SHADER_USER_DATA_PS_10";
        case mmSPI_SHADER_USER_DATA_PS_11:
            return"mmSPI_SHADER_USER_DATA_PS_11";
        case mmSPI_SHADER_USER_DATA_PS_12:
            return"mmSPI_SHADER_USER_DATA_PS_12";
        case mmSPI_SHADER_USER_DATA_PS_13:
            return"mmSPI_SHADER_USER_DATA_PS_13";
        case mmSPI_SHADER_USER_DATA_PS_14:
            return"mmSPI_SHADER_USER_DATA_PS_14";
        case mmSPI_SHADER_USER_DATA_PS_15:
            return"mmSPI_SHADER_USER_DATA_PS_15";
        case mmCOMPUTE_TMPRING_SIZE:
            return"mmCOMPUTE_TMPRING_SIZE";
        case mmCOMPUTE_PGM_LO:
            return "mmCOMPUTE_PGM_LO";
        case mmCOMPUTE_PGM_HI:
            return "mmCOMPUTE_PGM_HI";
        case mmCOMPUTE_PGM_RSRC1:
            return "mmCOMPUTE_PGM_RSRC1";
        case mmCOMPUTE_PGM_RSRC2:
            return "mmCOMPUTE_PGM_RSRC2";
        case mmCOMPUTE_USER_DATA_0:
            return"mmCOMPUTE_USER_DATA_0";
        case mmCOMPUTE_USER_DATA_1:
            return"mmCOMPUTE_USER_DATA_1";
        case mmCOMPUTE_USER_DATA_2:
            return"mmCOMPUTE_USER_DATA_2";
        case mmCOMPUTE_USER_DATA_3:
            return"mmCOMPUTE_USER_DATA_3";
        case mmCOMPUTE_USER_DATA_4:
            return"mmCOMPUTE_USER_DATA_4";
        case mmCOMPUTE_USER_DATA_5:
            return"mmCOMPUTE_USER_DATA_5";
        case mmCOMPUTE_USER_DATA_6:
            return"mmCOMPUTE_USER_DATA_6";
        case mmCOMPUTE_USER_DATA_7:
            return"mmCOMPUTE_USER_DATA_7";
        case mmCOMPUTE_USER_DATA_8:
            return"mmCOMPUTE_USER_DATA_8";
        case mmCOMPUTE_USER_DATA_9:
            return"mmCOMPUTE_USER_DATA_9";
        case mmCOMPUTE_USER_DATA_10:
            return"mmCOMPUTE_USER_DATA_10";
        case mmCOMPUTE_USER_DATA_11:
            return"mmCOMPUTE_USER_DATA_11";
        case mmCOMPUTE_USER_DATA_12:
            return"mmCOMPUTE_USER_DATA_12";
        case mmCOMPUTE_USER_DATA_13:
            return "mmCOMPUTE_USER_DATA_13";
        case mmCOMPUTE_USER_DATA_14:
            return "mmCOMPUTE_USER_DATA_14";
        case mmCOMPUTE_USER_DATA_15:
            return "mmCOMPUTE_USER_DATA_15";
        case mmCOMPUTE_NUM_THREAD_X:
            return "mmCOMPUTE_NUM_THREAD_X";
        case mmCOMPUTE_NUM_THREAD_Y:
            return "mmCOMPUTE_NUM_THREAD_Y";
        case mmCOMPUTE_NUM_THREAD_Z:
            return "mmCOMPUTE_NUM_THREAD_Z";
        case mmCOMPUTE_STATIC_THREAD_MGMT_SE0:
            return "mmCOMPUTE_STATIC_THREAD_MGMT_SE0";
        case mmCOMPUTE_STATIC_THREAD_MGMT_SE1:
            return "mmCOMPUTE_STATIC_THREAD_MGMT_SE1";
        case mmCOMPUTE_RESOURCE_LIMITS:
            return "mmCOMPUTE_RESOURCE_LIMITS";
        case mmSPI_SHADER_USER_DATA_VS_0:
            return"mmSPI_SHADER_USER_DATA_VS_0";
        case mmSPI_SHADER_USER_DATA_VS_1:
            return"mmSPI_SHADER_USER_DATA_VS_1";
        case mmSPI_SHADER_USER_DATA_VS_2:
            return"mmSPI_SHADER_USER_DATA_VS_2";
        case mmSPI_SHADER_USER_DATA_VS_3:
            return"mmSPI_SHADER_USER_DATA_VS_3";
        case mmSPI_SHADER_USER_DATA_VS_4:
            return"mmSPI_SHADER_USER_DATA_VS_4";
        case mmSPI_SHADER_USER_DATA_VS_5:
            return"mmSPI_SHADER_USER_DATA_VS_5";
        case mmSPI_SHADER_USER_DATA_VS_6:
            return"mmSPI_SHADER_USER_DATA_VS_6";
        case mmSPI_SHADER_USER_DATA_VS_7:
            return"mmSPI_SHADER_USER_DATA_VS_7";
        case mmSPI_SHADER_USER_DATA_VS_8:
            return"mmSPI_SHADER_USER_DATA_VS_8";
        case mmSPI_SHADER_USER_DATA_VS_9:
            return"mmSPI_SHADER_USER_DATA_VS_9";
        case mmSPI_SHADER_USER_DATA_VS_10:
            return"mmSPI_SHADER_USER_DATA_VS_10";
        case mmSPI_SHADER_USER_DATA_VS_11:
            return"mmSPI_SHADER_USER_DATA_VS_11";
        case mmSPI_SHADER_USER_DATA_VS_12:
            return"mmSPI_SHADER_USER_DATA_VS_12";
        case mmSPI_SHADER_USER_DATA_VS_13:
            return"mmSPI_SHADER_USER_DATA_VS_13";
        case mmSPI_SHADER_USER_DATA_VS_14:
            return"mmSPI_SHADER_USER_DATA_VS_14";
        case mmSPI_SHADER_USER_DATA_VS_15:
            return"mmSPI_SHADER_USER_DATA_VS_15";
        case mmSPI_SHADER_USER_DATA_HS_0:
            return"mmSPI_SHADER_USER_DATA_HS_0";
        case mmSPI_SHADER_USER_DATA_HS_1:
            return"mmSPI_SHADER_USER_DATA_HS_1";
        case mmSPI_SHADER_USER_DATA_HS_9:
            return"mmSPI_SHADER_USER_DATA_HS_9";
        case mmSPI_SHADER_PGM_RSRC3_GS__CI__VI:
            return"mmSPI_SHADER_PGM_RSRC3_GS__CI__VI";
        case mmSPI_SHADER_PGM_RSRC3_ES__CI__VI:
            return"mmSPI_SHADER_PGM_RSRC3_ES__CI__VI";
        case mmSPI_SHADER_PGM_RSRC3_LS__CI__VI:
            return"mmSPI_SHADER_PGM_RSRC3_LS__CI__VI";
        case mmSPI_SHADER_LATE_ALLOC_VS__CI__VI:
            return"mmSPI_SHADER_LATE_ALLOC_VS__CI__VI";
        default:
            break;
    }

    return "<UNK>";
}
