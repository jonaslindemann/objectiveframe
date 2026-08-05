#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class ColorScaleWindow : public UiWindow {
private:
    FemViewWindow *m_view;

public:
    ColorScaleWindow(const std::string name);
    virtual ~ColorScaleWindow();

    void setView(FemViewWindow *view);

    static std::shared_ptr<ColorScaleWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ColorScaleWindow> ColorScaleWindowPtr;

} // namespace ofui
