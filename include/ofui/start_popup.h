#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/popup_window.h>

class FemViewWindow;

namespace ofui {

class StartPopup : public PopupWindow {
private:
    FemViewWindow *m_view;

public:
    StartPopup(const std::string name, bool modal = true);
    virtual ~StartPopup();

    static std::shared_ptr<StartPopup> create(const std::string name, bool modal = true);

    void setView(FemViewWindow *view);

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<StartPopup> StartPopupPtr;

} // namespace ofui
