#ifndef BRAZOR_GUI_CMDLIST_H_
#define BRAZOR_GUI_CMDLIST_H_

#include "gui.h"


struct wt_cmdlist final : public widget_base
{
    void init() override;
    void render(double dt) override;
};

#endif // BRAZOR_GUI_CMDLIST_H_
