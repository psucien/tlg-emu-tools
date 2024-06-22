#include "gui_cmdlist.h"
#include "brazor.h"

#include <common/cmdproc_iface.h>

#undef min
#undef max

#define LITTLEENDIAN_CPU
#include <gcn/si_ci_vi_merged_enum.h>
#include <gcn/si_ci_vi_merged_typedef.h>
#include <gcn/si_ci_vi_merged_pm4defs.h>
#include <gcn/si_ci_vi_merged_pm4_it_opcodes.h>
#include <gcn/si_ci_vi_merged_offset.h>

#include <imgui/imgui.h>
#include <third/imgui_memory_editor.h>

#include <cstdint>
#include <vector>
#include <unordered_map>


using namespace Pal::Gfx6;

char const *get_opcode_name(uint32_t op);
char const* get_context_reg_name(uint32_t reg_offset);

static std::unordered_map<uint32_t, std::string> const nop_payload
{
    {0x68750013, "ACB SUBMIT MRK"},
    {0x68753000, "ALLOC ALIGN8"},
    {0x68750001, "PUSH MARKER"},
    {0x68750004, "SET VSHARP"},
    {0x68750005, "SET TSHARP"},
    {0x68750006, "SET SSHARP"},
    {0x6875000d, "SET USER DATA"},
};

//-----------------------------------------------------------------------------
void
on_nop
        ( PM4_TYPE_3_HEADER const  *pm4_hdr
        , uint32_t const           *it_body
        )
{
    ImGui::Separator();
    ImGui::BeginGroup();

    auto const *pkt =
        reinterpret_cast<PM4CMDNOP const *>(pm4_hdr);
    auto const *payload = &it_body[0];

    // Dump payload
    for (unsigned i = 0; i < pkt->header.count + 1; ++i) {
        std::string exp;
        if ((payload[i] & 0xffff0000) == 0x68750000) {
            const auto &e = nop_payload.find(payload[i]);
            if (e != nop_payload.cend()) {
                exp = {"(" + e->second + ")" };
            }
        }
        ImGui::Text("%02X: %08X %s", i, payload[i], exp.c_str());
    }

    ImGui::EndGroup();
}

//-----------------------------------------------------------------------------
void
on_dispatch
        ( PM4_TYPE_3_HEADER const  *pm4_hdr
        , uint32_t const           *it_body
        )
{
    ImGui::Separator();
    ImGui::BeginGroup();

    auto const *pkt =
        reinterpret_cast<PM4CMDDISPATCHDIRECT const *>(pm4_hdr);

    ImGui::Text("DIM_X    : %d", pkt->dimX);
    ImGui::Text("DIM_Y    : %d", pkt->dimY);
    ImGui::Text("DIM_Z    : %d", pkt->dimZ);
    ImGui::Text("INITIATOR: %X", pkt->dispatchInitiator.u32All);
    if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
        // TODO: dump_reg
        ImGui::EndTooltip();
    }

    ImGui::EndGroup();
}

//-----------------------------------------------------------------------------
void
on_set_base
        ( PM4_TYPE_3_HEADER const  *pm4_hdr
        , uint32_t const           *it_body
        )
{
    ImGui::Separator();
    ImGui::BeginGroup();

    auto const *pkt =
        reinterpret_cast<PM4CMDDRAWSETBASE const*>(pm4_hdr);
    ImGui::Text("BASE_INDEX: %08X", it_body[0]);
    ImGui::Text("ADDRESS0  : %08X", it_body[1]);
    ImGui::Text("ADDRESS1  : %08X", it_body[2]);

    ImGui::EndGroup();
}

//-----------------------------------------------------------------------------
auto parse__CB_COLOR_INFO = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<regCB_COLOR0_INFO const &>(value);

    if (ImGui::BeginTable("CB_COLOR_INFO", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ENDIAN");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ENDIAN);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FORMAT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FORMAT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("LINEAR_GENERAL");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.LINEAR_GENERAL);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("NUMBER_TYPE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.NUMBER_TYPE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COMP_SWAP");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.COMP_SWAP);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FAST_CLEAR");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FAST_CLEAR);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COMPRESSION");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.COMPRESSION);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("BLEND_CLAMP");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.BLEND_CLAMP);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("BLEND_BYPASS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.BLEND_BYPASS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("SIMPLE_FLOAT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.SIMPLE_FLOAT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ROUND_MODE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ROUND_MODE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CMASK_IS_LINEAR");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.CMASK_IS_LINEAR);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("BLEND_OPT_DONT_RD_DST");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.BLEND_OPT_DONT_RD_DST);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("BLEND_OPT_DISCARD_PIXEL");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.BLEND_OPT_DISCARD_PIXEL);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FMASK_COMPRESSION_DISABLE__CI__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FMASK_COMPRESSION_DISABLE__CI__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FMASK_COMPRESS_1FRAG_ONLY__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FMASK_COMPRESS_1FRAG_ONLY__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DCC_ENABLE__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DCC_ENABLE__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CMASK_ADDR_TYPE__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.CMASK_ADDR_TYPE__VI);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__CB_COLOR_ATTRIB = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<regCB_COLOR0_ATTRIB const &>(value);

    if (ImGui::BeginTable("CB_COLOR_ATTRIB", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TILE_MODE_INDEX");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.TILE_MODE_INDEX);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FMASK_TILE_MODE_INDEX");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FMASK_TILE_MODE_INDEX);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FMASK_BANK_HEIGHT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FMASK_BANK_HEIGHT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("NUM_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.NUM_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("NUM_FRAGMENTS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.NUM_FRAGMENTS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FORCE_DST_ALPHA_1");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FORCE_DST_ALPHA_1);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__CB_BLEND_CONTROL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<regCB_BLEND0_CONTROL const &>(value);

    if (ImGui::BeginTable("CB_BLEND_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        static const char* blends[] = {
            "BLEND_ZERO",
            "BLEND_ONE",
            "BLEND_SRC_COLOR",
            "BLEND_ONE_MINUS_SRC_COLOR",
            "BLEND_SRC_ALPHA",
            "BLEND_ONE_MINUS_SRC_ALPHA",
            "BLEND_DST_ALPHA",
            "BLEND_ONE_MINUS_DST_ALPHA",
            "BLEND_DST_COLOR",
            "BLEND_ONE_MINUS_DST_COLOR",
            "BLEND_SRC_ALPHA_SATURATE",
            "-ERR-",
            "-ERR-",
            "BLEND_CONSTANT_COLOR",
            "BLEND_ONE_MINUS_CONSTANT_COLOR",
            "BLEND_SRC1_COLOR",
            "BLEND_INV_SRC1_COLOR",
            "BLEND_SRC1_ALPHA",
            "BLEND_INV_SRC1_ALPHA",
            "BLEND_CONSTANT_ALPHA",
            "BLEND_ONE_MINUS_CONSTANT_ALPHA"
        };

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COLOR_SRCBLEND");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X (%s)",
            reg.bits.COLOR_SRCBLEND,
            blends[reg.bits.COLOR_SRCBLEND]
        );

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COLOR_COMB_FCN");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.COLOR_COMB_FCN);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COLOR_DESTBLEND");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X (%s)",
            reg.bits.COLOR_DESTBLEND,
            blends[reg.bits.COLOR_DESTBLEND]
        );

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALPHA_SRCBLEND");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALPHA_SRCBLEND);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALPHA_COMB_FCN");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALPHA_COMB_FCN);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALPHA_DESTBLEND");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALPHA_DESTBLEND);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("SEPARATE_ALPHA_BLEND");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.SEPARATE_ALPHA_BLEND);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DISABLE_ROP3");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DISABLE_ROP3);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__CB_COLOR_CONTROL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<CB_COLOR_CONTROL const &>(value);

    if (ImGui::BeginTable("CB_COLOR_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        static const char* mode[] = {
            "CB_DISABLE",
            "CB_NORMAL",
            "CB_ELIMINATE_FAST_CLEAR",
            "CB_RESOLVE",
            "-ERR-",
            "CB_FMASK_DECOMPRESS",
        };

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DISABLE_DUAL_QUAD__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DISABLE_DUAL_QUAD__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DEGAMMA_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DEGAMMA_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MODE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X (%s)",
            reg.bits.MODE,
            mode[reg.bits.MODE]);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ROP3");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ROP3);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__PA_CL_VTE_CNTL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<PA_CL_VTE_CNTL const &>(value);

    if (ImGui::BeginTable("PA_CL_VTE_CNTL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_X_SCALE_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_X_SCALE_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_X_OFFSET_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_X_OFFSET_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_Y_SCALE_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_Y_SCALE_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_Y_OFFSET_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_Y_OFFSET_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_Z_SCALE_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_Z_SCALE_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VPORT_Z_OFFSET_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VPORT_Z_OFFSET_ENA);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VTX_XY_FMT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VTX_XY_FMT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VTX_Z_FMT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VTX_Z_FMT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VTX_W0_FMT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VTX_W0_FMT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PERFCOUNTER_REF");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.PERFCOUNTER_REF);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__PA_SC_AA_CONFIG = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<regPA_SC_AA_CONFIG const &>(value);

    if (ImGui::BeginTable("PA_SC_AA_CONFIG", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MSAA_NUM_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MSAA_NUM_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("AA_MASK_CENTROID_DTMN");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.AA_MASK_CENTROID_DTMN);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MAX_SAMPLE_DIST");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MAX_SAMPLE_DIST);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MSAA_EXPOSED_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MSAA_EXPOSED_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DETAIL_TO_EXPOSED_MODE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DETAIL_TO_EXPOSED_MODE);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__DB_RENDER_CONTROL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<DB_RENDER_CONTROL const &>(value);

    if (ImGui::BeginTable("DB_RENDER_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DEPTH_CLEAR_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DEPTH_CLEAR_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCIL_CLEAR_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCIL_CLEAR_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DEPTH_COPY");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DEPTH_COPY);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCIL_COPY");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCIL_COPY);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("RESUMMARIZE_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.RESUMMARIZE_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCIL_COMPRESS_DISABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCIL_COMPRESS_DISABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DEPTH_COMPRESS_DISABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DEPTH_COMPRESS_DISABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COPY_CENTROID");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.COPY_CENTROID);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("COPY_SAMPLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.COPY_SAMPLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DECOMPRESS_ENABLE__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DECOMPRESS_ENABLE__VI);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__DB_DEPTH_CONTROL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<DB_DEPTH_CONTROL const &>(value);

    if (ImGui::BeginTable("DB_DEPTH_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCIL_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCIL_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Z_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.Z_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Z_WRITE_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.Z_WRITE_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DEPTH_BOUNDS_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DEPTH_BOUNDS_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ZFUNC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ZFUNC);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("BACKFACE_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.BACKFACE_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCILFUNC");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCILFUNC);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STENCILFUNC_BF");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STENCILFUNC_BF);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ENABLE_COLOR_WRITES_ON_DEPTH_FAIL");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ENABLE_COLOR_WRITES_ON_DEPTH_FAIL);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DISABLE_COLOR_WRITES_ON_DEPTH_PASS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DISABLE_COLOR_WRITES_ON_DEPTH_PASS);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__DB_Z_INFO = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<DB_Z_INFO const &>(value);

    if (ImGui::BeginTable("DB_DEPTH_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FORMAT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FORMAT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("NUM_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.NUM_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TILE_SPLIT__CI__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.TILE_SPLIT__CI__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TILE_MODE_INDEX");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.TILE_MODE_INDEX);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DECOMPRESS_ON_N_ZPLANES__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.DECOMPRESS_ON_N_ZPLANES__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALLOW_EXPCLEAR");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALLOW_EXPCLEAR);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("READ_SIZE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.READ_SIZE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("TILE_SURFACE_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.TILE_SURFACE_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CLEAR_DISALLOWED__VI");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.CLEAR_DISALLOWED__VI);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ZRANGE_PRECISION");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ZRANGE_PRECISION);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__DB_EQAA = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<DB_EQAA const &>(value);

    if (ImGui::BeginTable("DB_DEPTH_CONTROL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MAX_ANCHOR_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MAX_ANCHOR_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PS_ITER_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.PS_ITER_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MASK_EXPORT_NUM_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MASK_EXPORT_NUM_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALPHA_TO_MASK_NUM_SAMPLES");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALPHA_TO_MASK_NUM_SAMPLES);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("HIGH_QUALITY_INTERSECTIONS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.HIGH_QUALITY_INTERSECTIONS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("INCOHERENT_EQAA_READS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.INCOHERENT_EQAA_READS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("INTERPOLATE_COMP_Z");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.INTERPOLATE_COMP_Z);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("INTERPOLATE_SRC_Z");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.INTERPOLATE_SRC_Z);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("STATIC_ANCHOR_ASSOCIATIONS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.STATIC_ANCHOR_ASSOCIATIONS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ALPHA_TO_MASK_EQAA_DISABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ALPHA_TO_MASK_EQAA_DISABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("OVERRASTERIZATION_AMOUNT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.OVERRASTERIZATION_AMOUNT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("ENABLE_POSTZ_OVERRASTERIZATION");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.ENABLE_POSTZ_OVERRASTERIZATION);

        ImGui::EndTable();
    }
};

//-----------------------------------------------------------------------------
auto parse__PA_SU_SC_MODE_CNTL = [](uint32_t value)
{
    auto const reg =
        reinterpret_cast<PA_SU_SC_MODE_CNTL const &>(value);

    if (ImGui::BeginTable("PA_SU_SC_MODE_CNTL", 2,
        ImGuiTableFlags_Borders
        | ImGuiTableFlags_RowBg))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CULL_FRONT");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.CULL_FRONT);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("CULL_BACK");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.CULL_BACK);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("FACE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.FACE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLY_MODE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLY_MODE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLYMODE_FRONT_PTYPE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLYMODE_FRONT_PTYPE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLYMODE_BACK_PTYPE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLYMODE_BACK_PTYPE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLY_OFFSET_FRONT_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLY_OFFSET_FRONT_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLY_OFFSET_BACK_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLY_OFFSET_BACK_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("POLY_OFFSET_PARA_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.POLY_OFFSET_PARA_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("VTX_WINDOW_OFFSET_ENABLE");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.VTX_WINDOW_OFFSET_ENABLE);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PROVOKING_VTX_LAST");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.PROVOKING_VTX_LAST);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PERSP_CORR_DIS");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.PERSP_CORR_DIS);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("MULTI_PRIM_IB_ENA");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%X", reg.bits.MULTI_PRIM_IB_ENA);

        ImGui::EndTable();
    }
};

//----------------------------------------------------------------------------
void
on_set_context_reg
        ( PM4_TYPE_3_HEADER const   *pm4_hdr
        , uint32_t const            *it_body
        )
{
    ImGui::Separator();
    ImGui::BeginGroup();

    auto const *pkt =
        reinterpret_cast<PM4CMDSETDATA const*>(pm4_hdr);

    for (auto i = 0u; i < pm4_hdr->count; ++i)
    {
        auto const absOffset = CONTEXT_SPACE_START + pkt->regOffset + i;
        extern char const *get_context_reg_name(uint32_t);
        ImGui::Text(
                "reg: %4X (%s)",
                pkt->regOffset + i,
                get_context_reg_name(absOffset)
        );
        ImGui::Text(
                "[%08X]",
                it_body[i + 1]
        );

        switch (absOffset) {
            case mmPA_SU_SC_MODE_CNTL: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__PA_SU_SC_MODE_CNTL(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmPA_SC_AA_CONFIG: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__PA_SC_AA_CONFIG(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmPA_CL_VTE_CNTL: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__PA_CL_VTE_CNTL(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmCB_COLOR_CONTROL:
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__CB_COLOR_CONTROL(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            case mmCB_COLOR0_INFO:
                [[fallthrough]];
            case mmCB_COLOR1_INFO:
                [[fallthrough]];
            case mmCB_COLOR2_INFO:
                [[fallthrough]];
            case mmCB_COLOR3_INFO:
                [[fallthrough]];
            case mmCB_COLOR4_INFO:
                [[fallthrough]];
            case mmCB_COLOR5_INFO:
                [[fallthrough]];
            case mmCB_COLOR6_INFO:
                [[fallthrough]];
            case mmCB_COLOR7_INFO: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__CB_COLOR_INFO(it_body[i + 1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmCB_COLOR0_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR1_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR2_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR3_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR4_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR5_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR6_ATTRIB:
                [[fallthrough]];
            case mmCB_COLOR7_ATTRIB: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__CB_COLOR_ATTRIB(it_body[i + 1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmCB_BLEND0_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND1_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND2_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND3_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND4_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND5_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND6_CONTROL:
                [[fallthrough]];
            case mmCB_BLEND7_CONTROL: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__CB_BLEND_CONTROL(it_body[i + 1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmDB_RENDER_CONTROL: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__DB_RENDER_CONTROL(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmDB_DEPTH_CONTROL: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__DB_DEPTH_CONTROL(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmDB_EQAA: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__DB_EQAA(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmDB_Z_INFO: {
                if (ImGui::IsItemHovered() && ImGui::BeginTooltip()) {
                    parse__DB_Z_INFO(it_body[1]);
                    ImGui::EndTooltip();
                }
                break;
            }
            case mmDB_HTILE_DATA_BASE: {
                auto const &reg =
                    reinterpret_cast<DB_HTILE_DATA_BASE const&>(it_body[1]);
                ImGui::Text(
                    "addr: %08x",
                    reg.bits.BASE_256B * 256u
                );
                break;
            }
            default:
                break;
        }

        //...
    }

    ImGui::EndGroup();
}

//----------------------------------------------------------------------------
void
on_set_sh_reg
        ( PM4_TYPE_3_HEADER const   *pm4_hdr
        , uint32_t const            *it_body
        )
{
    ImGui::Separator();
    ImGui::BeginGroup();

    auto const *pkt =
        reinterpret_cast<PM4CMDSETDATA const*>(pm4_hdr);

    for (auto i = 0u; i < pm4_hdr->count; ++i)
    {
        auto const absOffset = PERSISTENT_SPACE_START + pkt->regOffset + i;
        extern char const *get_shader_reg_name(uint32_t);
        ImGui::Text(
                "reg: %4X (%s)",
                pkt->regOffset + i,
                get_shader_reg_name(absOffset)
        );

        ImGui::Text(
                "%08X",
                *((uint32_t*)(pm4_hdr) + 2 + i)
        );

        //...
    }

    ImGui::EndGroup();
}

//----------------------------------------------------------------------------
struct dbg_cmdlist final : public i_cmdproc
{
    MemoryEditor    cmdb_view{};
    MemoryEditor    lds_view{};
    uint32_t        vqid{};

    struct batch_info_t {
        std::string         marker{};
        size_t              start_addr;
        size_t              end_addr;
        size_t              command_addr;
        uint32_t            type;
        bool                bypass{ false };
    };

    std::vector<batch_info_t> batches{};
    std::vector<std::pair<uint64_t, uint64_t>> labels{};

    uintptr_t   last_cmdb_addr{ 0 };
    size_t      last_cmdb_size{ 0 };
    int         batch_bp{ -1 };


    dbg_cmdlist()
    {
        cmdb_view.ReadOnly = true;
        cmdb_view.Open     = false;
    }

    result_t
    exec_cmdb
            ( uintptr_t cmdb_addr
            , size_t    cmdb_size
            )
    {
        // analyze stream for batches
        if (   cmdb_addr != last_cmdb_addr
            || cmdb_size != last_cmdb_size) {
            auto const *pm4_hdr =
                reinterpret_cast<PM4_HEADER const*>(cmdb_addr);

            size_t processed_size = 0;
            size_t prev_offset = 0;

            std::string marker{};

            while (processed_size < cmdb_size) {
                auto *next_pm4_hdr = get_next(pm4_hdr, 1);
                auto  processed_len =
                    reinterpret_cast<uintptr_t>(next_pm4_hdr) - reinterpret_cast<uintptr_t>(pm4_hdr);
                processed_size += processed_len;

                if (pm4_hdr->type == PM4_TYPE_3) {

                    auto const *pm4_t3 =
                        reinterpret_cast<PM4_TYPE_3_HEADER const*>(pm4_hdr);

                    if (pm4_t3->opcode == IT_NOP) {
                        auto const* it_body =
                            reinterpret_cast<uint32_t const*>(pm4_hdr + 1);
                        if (it_body[0] == 0x68750001) {
                            marker = { (char*)&it_body[1]};
                        }
                    }

                    if (pm4_t3->opcode == IT_DISPATCH_DIRECT
                        || pm4_t3->opcode == IT_DISPATCH_INDIRECT
                        || pm4_t3->opcode == IT_DRAW_INDEX_2
                        || pm4_t3->opcode == IT_DRAW_INDEX_AUTO
                        || pm4_t3->opcode == IT_DRAW_INDEX_OFFSET_2
                        || pm4_t3->opcode == IT_DRAW_INDEX_INDIRECT
                        // ...
                        )
                    {
                        // All these commands are terminated by NOP at the end, so
                        // it is safe to skip it to be even with CP
                        next_pm4_hdr = get_next(next_pm4_hdr, 1);
                        auto constexpr nop_len = 0x10;
                        processed_len += nop_len;
                        processed_size += nop_len;

                        batches.emplace_back(batch_info_t{
                            marker,
                            prev_offset,
                            processed_size,
                            processed_size - processed_len,
                            pm4_t3->opcode
                        });
                        prev_offset = processed_size;
                        marker.clear();
                    }
                }

                pm4_hdr = next_pm4_hdr;
            }

            // state batch (last)
            if (processed_size - prev_offset > 0) {
                batches.emplace_back(batch_info_t{
                    marker,
                    prev_offset,
                    processed_size,
                    0,
                    0xff
                });
            }

            last_cmdb_addr = cmdb_addr;
            last_cmdb_size = cmdb_size;
        }

        auto queue_name =
            std::string{ vqid > 254 ? "GFX" : "ASC" };

        if (vqid < 254) {
            queue_name += std::to_string(vqid);
        }

        ImGui::Begin(queue_name.c_str());
        {
            ImGui::Text("queue    : %s", queue_name.c_str());
            ImGui::Text("base addr: %08lX", cmdb_addr);
            ImGui::SameLine();
            if (ImGui::SmallButton(">")) {
                cmdb_view.Open = true;
            }
            ImGui::Text("size     : %04lX",   cmdb_size);
            ImGui::Separator();

            char batch_hdr[128];
            for (int batch_id = 0; batch_id < batches.size(); ++batch_id) {
                auto processed_size = 0ull;
                auto const *pm4_hdr =
                    reinterpret_cast<PM4_HEADER const*>(cmdb_addr + batches[batch_id].start_addr);

                sprintf(
                    batch_hdr,
                    "%08llX: batch-%03d | %s",
                    cmdb_addr + batches[batch_id].start_addr,
                    batch_id,
                    batches[batch_id].marker.c_str()
                );

                if (batch_id == batch_bp) { // highlight batch at breakpoint
                    ImGui::PushStyleColor(
                        ImGuiCol_Header,
                        ImVec4{ 1.0f, 0.5f, 0.5f, 0.5f }
                    );
                }

                if (batches[batch_id].type == 0xff || ImGui::CollapsingHeader(batch_hdr)) {
                    auto const batch_sz =
                        batches[batch_id].end_addr - batches[batch_id].start_addr;
                    while (processed_size < batch_sz) {
                        auto op = 0xffu;

                        if (pm4_hdr->type == PM4_TYPE_3) {
                            auto const* pm4_t3 =
                                reinterpret_cast<PM4_TYPE_3_HEADER const*>(pm4_hdr);
                            op = pm4_t3->opcode;

                            static char header_name[128];
                            sprintf(
                                header_name,
                                "%08llX: %s",
                                cmdb_addr + batches[batch_id].start_addr + processed_size,
                                get_opcode_name(op)
                            );

                            if (ImGui::TreeNode(header_name)) {
                                if (ImGui::BeginTable("split", 1)) {
                                    ImGui::TableNextColumn(); ImGui::Text("size: %d", pm4_hdr->count + 1);

                                    cmdb_view.GotoAddrAndHighlight(
                                        reinterpret_cast<size_t>(pm4_hdr) - cmdb_addr,
                                        reinterpret_cast<size_t>(pm4_hdr) - cmdb_addr + (pm4_hdr->count + 2) * 4
                                    );

                                    auto const* it_body =
                                        reinterpret_cast<uint32_t const*>(pm4_hdr + 1);

                                    switch (op) {
                                        case IT_NOP: {
                                            on_nop(pm4_t3, it_body);
                                            break;
                                        }
                                        case IT_SET_BASE: {
                                            on_set_base(pm4_t3, it_body);
                                            break;
                                        }
                                        case IT_SET_CONTEXT_REG: {
                                            on_set_context_reg(pm4_t3, it_body);
                                            break;
                                        }
                                        case IT_SET_SH_REG: {
                                            on_set_sh_reg(pm4_t3, it_body);
                                            break;
                                        }
                                        case IT_DISPATCH_DIRECT: {
                                            on_dispatch(pm4_t3, it_body);
                                            break;
                                        }
                                        default: {
                                            auto const *payload = &it_body[0];
                                            for (unsigned i = 0; i < pm4_hdr->count + 1; ++i) {
                                                ImGui::Text("%02X: %08X", i, payload[i]);
                                            }
                                        }
                                    }

                                    ImGui::EndTable();
                                }
                                ImGui::TreePop();
                            }
                        }
                        else {
                            ImGui::Text("<UNK PACKET>");
                        }

                        auto const *next_pm4_hdr = get_next(pm4_hdr, 1);
                        auto const processed_len =
                            reinterpret_cast<uintptr_t>(next_pm4_hdr) - reinterpret_cast<uintptr_t>(pm4_hdr);
                        pm4_hdr = next_pm4_hdr;
                        processed_size += processed_len;
                    }
                }

                if (batch_id == batch_bp) {
                    ImGui::PopStyleColor();
                }

                if (batch_id == batches.size() - 2) {
                    ImGui::Separator();
                }
            }
        }
        ImGui::End();

        if (cmdb_view.Open) {
            MemoryEditor::Sizes s;
            cmdb_view.CalcSizes(s, cmdb_size, cmdb_addr);
            ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(s.WindowWidth, FLT_MAX));

            if (ImGui::Begin("[GFX] command buffer", &cmdb_view.Open, ImGuiWindowFlags_NoScrollbar)) {
                cmdb_view.DrawContents(
                    reinterpret_cast<void*>(cmdb_addr),
                    cmdb_size,
                    cmdb_addr
                );
            }
            ImGui::End();
        }

        return result_t::eDone;
    }
};

dbg_cmdlist dbg_gfx{};

//----------------------------------------------------------------------------
void
render_dbg_cmdlist
        ( uintptr_t     ptr
        , size_t        sz
        )
{
    dbg_gfx.vqid = 255;
    dbg_gfx.exec_cmdb(
        ptr,
        sz
    );
}
