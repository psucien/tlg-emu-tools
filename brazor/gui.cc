#include "gui.h"

std::vector<widget_base*> widgets{};

//------------------------------------------------------------------------------
widget_base::widget_base
        ( std::string const &title
        )
    : name{ title }
{
    widgets.emplace_back(this);
}
