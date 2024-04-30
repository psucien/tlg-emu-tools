#ifndef BRAZOR_GUI_HELPERS_H_
#define BRAZOR_GUI_HELPERS_H_

#include <string>
#include <vector>


struct widget_base
{
    widget_base() = delete;
    explicit widget_base(std::string const &title);
    virtual ~widget_base() {}

    virtual void init() = 0;
    virtual void render(double dt) = 0;

    std::string name{};
};

extern std::vector<widget_base*> widgets;

bool aligned_button(char const *label, float alignment = 0.5f);

#endif // BRAZOR_GUI_HELPERS_H_
