#pragma once

#include <ofui/texture.h>
#include <ofui/ui_window.h>

#include <vector>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

enum class OfToolbarButtonType {
    Button,
    ToggleButton,
    RadioButton
};

class OfToolbarButton {
private:
    std::string m_name;
    OfToolbarButtonType m_type;
    bool m_selected;
    bool m_enabled;
    int m_group;
    int m_id;
    ofui::TexturePtr m_texture;

public:
    OfToolbarButton(const std::string name = "Button", OfToolbarButtonType type = OfToolbarButtonType::Button,
                    ofui::TexturePtr texture = nullptr, int group = -1, int id = -1);
    virtual ~OfToolbarButton() = default;

    void setSelected(bool flag);
    bool selected();
    void toggleSelected();
    void select();
    void unselect();

    void setEnabled(bool flag);
    bool enabled();
    void enable();
    void disable();

    int group();
    int id();

    ofui::TexturePtr texture();

    OfToolbarButtonType type();
    std::string name();
    void setName(const std::string name);
};

class ToolbarWindow : public UiWindow {
private:
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif

    int m_selectedButton;

    std::vector<OfToolbarButton> m_buttons;
    ofui::TexturePtr m_texture;

public:
    ToolbarWindow(const std::string name);
    virtual ~ToolbarWindow();

#ifdef USE_FEMVIEW
    void setView(FemViewWindow *view);
#else
    void setWidget(FemWidget *femWidget);
#endif

    void addButton(const std::string name, OfToolbarButtonType type = OfToolbarButtonType::Button,
                   std::string filename = "", int group = -1);

    OfToolbarButton &button(int idx);
    size_t buttonCount();

    void update();

    static std::shared_ptr<ToolbarWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ToolbarWindow> MainToolbarPtr;

} // namespace ofui
