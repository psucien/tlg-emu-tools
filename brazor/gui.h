#ifndef BRAZOR_GUI_HELPERS_H_
#define BRAZOR_GUI_HELPERS_H_

#include <string>


struct brazor_ui_element
{
    brazor_ui_element() {}
    virtual ~brazor_ui_element() {}

    virtual void init(std::string const &title) = 0;
    virtual void render() = 0;
};

bool aligned_button(char const *label, float alignment = 0.5f);

#endif // BRAZOR_GUI_HELPERS_H_
