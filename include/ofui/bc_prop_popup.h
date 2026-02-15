#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/popup_window.h>

class FemViewWindow;

namespace ofui {

class BCPropPopup : public PopupWindow {
private:
    FemViewWindow *m_view;
    std::array<char, 255> m_nameArr;
    int m_color;
    bool m_prescribedDofs[6];
    double m_prescribedValues[6];

public:
    BCPropPopup(const std::string name, bool modal);
    virtual ~BCPropPopup();

    static std::shared_ptr<BCPropPopup> create(const std::string name, bool modal = true);

    void setFemView(::FemViewWindow *view);

    void update();

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<BCPropPopup> BCPropPopupPtr;
} // namespace ofui
