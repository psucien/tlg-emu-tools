#include "gui_menu.h"
#include "brazor.h"

#include <imgui/imgui.h>


wt_main_menu tool_menu{};

void
wt_main_menu::render
        ( double dt
        )
{
    auto const *iblock_ptr =
        reinterpret_cast<shmem_info_block_header const*>(tool.iblock.ptr);

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("capture")) {
            if (ImGui::MenuItem("open", nullptr, false, !tool.cap_is_opened)) {
                extern std::string capture_selected;
                //tool.load_capture(capture_selected);
            }

            if (ImGui::MenuItem("dump", nullptr, false, tool.online_mode && tool.do_pause)) {
                //tool.dump_capture_compressed(iblock_ptr->title_name); // TODO: modal window for fname input
            }

            if (ImGui::MenuItem("close", nullptr, false, tool.cap_is_opened)) {
                //tool.close_capture();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("view")) {
            if (ImGui::MenuItem("memory", nullptr, false, tool.online_mode)) {

                // List of regions + hex view/edit
                // ...
            }

            if (ImGui::MenuItem("log", nullptr, false, tool.online_mode)) {

                // List of regions + hex view/edit
                // ...
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("opts")) {
            if (ImGui::Checkbox("pause emu", &tool.do_pause)) {
                tool.cv_pause.notify_all();
            }
            ImGui::Checkbox("render", &tool.do_render);
            ImGui::EndMenu();
        }

        if (tool.online_mode) {
            if (aligned_button("disconnect", 1.0f)) {
                tool.disconnect();
            }
        }
        else {
            if (aligned_button("connect", 1.0f)) {
                tool.online_mode = true;
            }
        }
        ImGui::EndMainMenuBar();
    }
}
