#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/popup_window.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class BCPropPopup : public PopupWindow {
private:
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    std::array<char, 255> m_nameArr;
    int m_color;
    bool m_prescribedDofs[6];
    double m_prescribedValues[6];

public:
    BCPropPopup(const std::string name, bool modal);
    virtual ~BCPropPopup();

    static std::shared_ptr<BCPropPopup> create(const std::string name, bool modal = true);

#ifdef USE_FEMVIEW
    void setFemView(FemViewWindow *view);
#else
    void setFemWidget(FemWidget *widget);
#endif

    void update();

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<BCPropPopup> BCPropPopupPtr;
} // namespace ofui
