#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include <ofui/popup_window.h>
#include <ofui/texture.h>

class FemViewWindow;

namespace ofui {

enum class OfStartButton {
    NewModel,
    OpenModel,
    OpenPythonModel,
    Documentation,
    StartAPI
};

typedef std::function<void(OfStartButton &button)> StartButtonClickedFunc;
typedef std::function<void(std::string &filename)> ExampleClickedFunc;

class StartPopup : public PopupWindow {
private:
    FemViewWindow *m_view;
    std::vector<std::string> m_exampleFilename;
    std::vector<TexturePtr> m_exampleImage;

    StartButtonClickedFunc m_startButtonClickedFunc;
    ExampleClickedFunc m_exampleClickedFunc;

public:
    StartPopup(const std::string name, bool modal = true);
    virtual ~StartPopup();

    static std::shared_ptr<StartPopup> create(const std::string name, bool modal = true);

    void setView(FemViewWindow *view);

    void addExample(const std::string &filename, const std::string &imageFilename);
    void assignStartButtonClickedFunc(StartButtonClickedFunc func);
    void assignExampleClickedFunc(ExampleClickedFunc func);

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<StartPopup> StartPopupPtr;

} // namespace ofui
