#ifndef BRAZOR_GUI_MENU_H_
#define BRAZOR_GUI_MENU_H_

#include "gui.h"


struct wt_main_menu final : public widget_base
{
    wt_main_menu() : widget_base{ "Main menu" } {}

    void init() override {};
    void render(double dt) override;
};

#endif // BRAZOR_GUI_MENU_H_
