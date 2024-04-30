#include "brazor.h"

#include <cstring>


ipc_manager g_ipc_manager{};

//-----------------------------------------------------------------------------
void
debugger_module::thread_func
        ( std::stop_token   stoken
        )
{
    bool close_request_received{ false };
    bool frame_done{ true };

    while (!stoken.stop_requested() && !close_request_received) {

        if (do_pause && frame_done /* && !capture_*/) { // stop right before the next frame processing
            std::unique_lock<std::mutex> lock{ m_pause };
            cv_pause.wait(lock, [&]() {
                return !do_pause
                    //|| capture_
                    || stoken.stop_requested();
                });
            continue; // to correctly handle thread stop request
        }

        // Advance capture dump
        //if (capture_) {
        //    capture_->process();
        //}

        ipc_packet pkt{};
        std::memset(&pkt, 0xcc, sizeof(pkt));

        if (!g_ipc_manager.client_read_packet(&pkt)) {
            continue;
        }

        // First command after `submitDone` starts the timer as it
        // can be only first command in a frame. The end mark is
        // written in FLIP IRQ handler.

        if (frame_done) {
            frame_start_t[0] =
                std::chrono::high_resolution_clock::now();
            frame_done = false;
        }

        //gpu().set_bypass(!do_render || capture_); // disable rendering during capturing

        switch (pkt.opcode) {
            case ipc_packet::opcode_t::eGFX_SUBMIT: {
#if 0
                if (capture_) {
                    for (auto cb_pair = 0u; cb_pair < pkt.gfx.count; ++cb_pair) {
                        capture_->register_cmdlist(
                            pkt.gfx.dcb_gpu_addrs[cb_pair],
                            pkt.gfx.dcb_sizes_bytes[cb_pair],
                            255
                        );

                        if (pkt.gfx.ccb_gpu_addrs[cb_pair] && pkt.gfx.ccb_sizes_bytes[cb_pair] > 0) {
                            capture_->register_cmdlist(
                                pkt.gfx.ccb_gpu_addrs[cb_pair],
                                pkt.gfx.ccb_sizes_bytes[cb_pair],
                                254
                            );
                        }
                    }
                }

                gpu_.submit_gfxq(
                    pkt.gfx.count,
                    pkt.gfx.dcb_gpu_addrs,
                    pkt.gfx.dcb_sizes_bytes,
                    pkt.gfx.ccb_gpu_addrs,
                    pkt.gfx.ccb_sizes_bytes
                );
#endif
                break;
            }

            case ipc_packet::opcode_t::eASC_SUBMIT: {
#if 0
                if (capture_) {
                    capture_->register_cmdlist(
                        pkt.asc.cmdb,
                        pkt.asc.size,
                        pkt.asc.vqid
                    );
                }

                gpu_.submit_acq(
                    pkt.asc.vqid,
                    pkt.asc.cmdb,
                    pkt.asc.size
                );
#endif
                break;
            }

            case ipc_packet::opcode_t::eFLIP_REQUEST: {
#if 0
                if (capture_) {
                    capture_->vo_buf_idx = pkt.flip.index;
                }

                if (pkt.flip.eop) {
                    assert(gpu_.flip_req.size() < 16u);
                    gpu_.flip_req.emplace(pkt.flip);
                }
                else {
                    flip(pkt.flip.index);
                }
#endif
                break;
            }

            case ipc_packet::opcode_t::eSUBMIT_DONE: {
#if 0
                if (capture_) {
                    capture_->finish();
                    capture_.release();
                }
#endif
                frame_done = true;
                break;
            }

            case ipc_packet::opcode_t::eCLOSE_CONNECTION: {
                close_request_received = true;
                break;
            }

            default: {
                break;
            }
        }
    }

    //gpu().wait_submissions_allowed();
}
