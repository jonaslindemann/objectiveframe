#pragma once

#include <ofui/texture.h>
#include <ofui/ui_window.h>

#include <functional>
#include <vector>

namespace ofui {

enum class OfToolbarButtonType {
    Button,
    ToggleButton,
    RadioButton,
    Spacer
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

typedef std::function<void(OfToolbarButton &button)> ButtonClickedFunc;
typedef std::function<void(OfToolbarButton &button)> ButtonHoverFunc;

class ToolbarWindow : public UiWindow {
private:
    int m_selectedButton;

    std::vector<OfToolbarButton> m_buttons;
    ofui::TexturePtr m_texture;
    ImVec4 m_selectedColor;
    ImVec4 m_color;

    ButtonClickedFunc m_onButtonClicked;
    ButtonHoverFunc m_onButtonHover;

    std::vector<std::shared_ptr<ToolbarWindow>> m_toolbarGroup;

public:
    ToolbarWindow(const std::string name);
    virtual ~ToolbarWindow();

    void addButton(const std::string name, OfToolbarButtonType type = OfToolbarButtonType::Button,
                   std::string filename = "", int group = -1);

    void addSpacer();

    void addToolbarGroup(std::shared_ptr<ToolbarWindow> toolbar);

    void selectButton(int idx, int group);

    OfToolbarButton &button(int idx);
    size_t buttonCount();

    void update();

    void assignOnButtonClicked(ofui::ButtonClickedFunc &onButtonClicked);
    void assignOnButtonHover(ofui::ButtonHoverFunc &onButtonHover);

    static std::shared_ptr<ToolbarWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ToolbarWindow> ToolbarWindowPtr;

} // namespace ofui
