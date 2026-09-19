#pragma once

#include <ofui/texture.h>
#include <ofui/ui_profile.h>
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

enum class ToolbarOrientation {
    Horizontal,
    Vertical
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
    UiFeature m_feature;

public:
    OfToolbarButton(const std::string name = "Button", OfToolbarButtonType type = OfToolbarButtonType::Button,
                    ofui::TexturePtr texture = nullptr, int group = -1, int id = -1,
                    UiFeature feature = UiFeature::None);
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

    /**
     * The interface feature this button belongs to.
     *
     * UiFeature::None for a button that is always shown, which is most of them.
     */
    UiFeature feature();

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

    ToolbarOrientation m_orientation{ToolbarOrientation::Horizontal};

public:
    ToolbarWindow(const std::string name);
    virtual ~ToolbarWindow();

    void setOrientation(ToolbarOrientation orientation);
    ToolbarOrientation orientation();

    void addButton(const std::string name, OfToolbarButtonType type = OfToolbarButtonType::Button,
                   std::string filename = "", int group = -1, UiFeature feature = UiFeature::None);

    void addSpacer();

    void addToolbarGroup(std::shared_ptr<ToolbarWindow> toolbar);

    void selectButton(int idx, int group);

    /**
     * Selects a button by name.
     *
     * Preferred over the index overload - spacers occupy button slots, so the
     * indices shift whenever a button or spacer is inserted.
     */
    void selectButton(const std::string name, int group);

    /**
     * Sets a toggle button's state without going through a click.
     *
     * selectButton() can only ever select, which is what a radio group needs.
     * A toggle mirrors something the application owns and has to be able to
     * follow it back to off.
     */
    void setButtonSelected(const std::string name, bool flag);

    OfToolbarButton &button(int idx);
    size_t buttonCount();

    void update();

    void assignOnButtonClicked(const ofui::ButtonClickedFunc &onButtonClicked);
    void assignOnButtonHover(const ofui::ButtonHoverFunc &onButtonHover);

    static std::shared_ptr<ToolbarWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ToolbarWindow> ToolbarWindowPtr;

} // namespace ofui
