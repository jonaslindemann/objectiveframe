#pragma once

#include <memory>
#include <string>

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class SelfWeightWindow : public UiWindow {
private:
    FemViewWindow *m_view;

    bool m_enabled;
    int m_mode;
    double m_gravity;
    double m_gravityScale;
    double m_totalWeight;

public:
    SelfWeightWindow(const std::string name);
    virtual ~SelfWeightWindow();

    static std::shared_ptr<SelfWeightWindow> create(const std::string name);

    void setFemView(FemViewWindow *view);

    void update();

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<SelfWeightWindow> SelfWeightWindowPtr;

} // namespace ofui
