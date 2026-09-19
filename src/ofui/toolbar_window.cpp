#include <ofui/toolbar_window.h>

#include <vfem/beam_model.h>

using namespace ofui;

ToolbarWindow::ToolbarWindow(const std::string name)
    : UiWindow(name), m_selectedButton{-1}, m_selectedColor{0.0, 0.0, 1.0, 1.0}, m_color{0.0, 0.0, 0.0, 1.0}
{
    setWindowFlags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    setCanClose(false);
    setAutoPlacement(false);

    m_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
    m_selectedColor = {1.0, 1.0, 1.0, 1.0};

    this->setIgnoreFocusCheck(true);
}

ToolbarWindow::~ToolbarWindow()
{}

void ofui::ToolbarWindow::setOrientation(ToolbarOrientation orientation)
{
    m_orientation = orientation;
}

ToolbarOrientation ofui::ToolbarWindow::orientation()
{
    return m_orientation;
}

void ofui::ToolbarWindow::addButton(const std::string name, OfToolbarButtonType type, std::string filename, int group,
                                    UiFeature feature)
{
    auto texture = Texture::create(filename);
    texture->load();
    m_buttons.emplace_back(name, type, texture, group, texture->id(), feature);
}
void ofui::ToolbarWindow::addSpacer()
{
    // Spacers carry no feature of their own - doDraw() collapses the ones left
    // dividing nothing once the group around them is hidden.

    m_buttons.emplace_back("", OfToolbarButtonType::Spacer, nullptr, -1, -1, UiFeature::None);
}
void ofui::ToolbarWindow::addToolbarGroup(std::shared_ptr<ToolbarWindow> toolbar)
{
    m_toolbarGroup.push_back(toolbar);
}

void ofui::ToolbarWindow::selectButton(int idx, int group)
{
    if (idx < 0 || idx >= m_buttons.size())
    {
        return;
    }

    if (group >= 0)
    {
        for (auto &button : m_buttons)
        {
            if (button.group() == group)
            {
                button.unselect();
            }
        }

        for (auto &toolbar : m_toolbarGroup)
        {
            for (auto &button : toolbar->m_buttons)
            {
                if (button.group() == group)
                {
                    button.unselect();
                }
            }
        }
    }

    m_buttons[idx].select();
    m_selectedButton = idx;
}
void ofui::ToolbarWindow::selectButton(const std::string name, int group)
{
    for (size_t i = 0; i < m_buttons.size(); i++)
    {
        if (m_buttons[i].name() == name)
        {
            this->selectButton(int(i), group);
            return;
        }
    }
}

void ofui::ToolbarWindow::setButtonSelected(const std::string name, bool flag)
{
    for (auto &button : m_buttons)
        if (button.name() == name)
            button.setSelected(flag);
}

ofui::OfToolbarButton &ofui::ToolbarWindow::button(int idx)
{
    return m_buttons[idx];
}

size_t ofui::ToolbarWindow::buttonCount()
{
    return m_buttons.size();
}

void ToolbarWindow::update()
{}

void ofui::ToolbarWindow::assignOnButtonClicked(const ButtonClickedFunc &onButtonClicked)
{
    m_onButtonClicked = onButtonClicked;
}

void ofui::ToolbarWindow::assignOnButtonHover(const ButtonHoverFunc &onButtonHover)
{
    m_onButtonHover = onButtonHover;
}

std::shared_ptr<ToolbarWindow> ToolbarWindow::create(const std::string name)
{
    return std::make_shared<ToolbarWindow>(name);
}

void ToolbarWindow::doDraw()
{
    // ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    const float scale = ImGui::GetIO().FontGlobalScale;
    ImVec2 button_sz(40.0f * scale, 40.0f * scale);

    auto *profile = UiProfile::instance();

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{1, 0, 0, 1});

    // Hiding a button leaves a hole in two places. The ImGui id has to stay
    // tied to the button's index in m_buttons, because that is what a radio
    // click selects by - deriving it from a counter that only advances for
    // drawn buttons would make every click past the first hidden one land on
    // its neighbour. And a spacer whose group has gone is a divider with
    // nothing on one side of it, so spacers are held back until a button
    // actually follows, and a run of them collapses to one gap.

    bool anyDrawn = false;
    bool spacerPending = false;

    for (size_t i = 0; i < m_buttons.size(); i++)
    {
        auto &button = m_buttons[i];

        if (!profile->has(button.feature()))
            continue;

        if (button.type() == OfToolbarButtonType::Spacer)
        {
            spacerPending = anyDrawn;
            continue;
        }

        if (anyDrawn && m_orientation == ToolbarOrientation::Horizontal)
            ImGui::SameLine();

        if (spacerPending)
        {
            ImGui::Dummy(ImVec2(10.0f * scale, 10.0f * scale));

            if (m_orientation == ToolbarOrientation::Horizontal)
                ImGui::SameLine();

            spacerPending = false;
        }

        ImGui::PushID(int(i));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4.0f * scale, 4.0f * scale});

        if (button.texture() != nullptr)
            button.texture()->bind();

        if (button.type() == OfToolbarButtonType::Button)
        {
            if (ImGui::ImageButton("", (ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), m_color,
                                   m_selectedColor))
            {
                if (m_onButtonClicked)
                {
                    m_onButtonClicked(button);
                }
            }
            if (ImGui::IsItemHovered())
            {
                if (m_onButtonHover)
                    m_onButtonHover(button);
            }
        }
        else if (button.type() == OfToolbarButtonType::ToggleButton)
        {
            // The same frame a selected radio button gets. A toggle that is on
            // and a mode that is active mean the same thing to the user, and
            // drawing them differently only invites the question of how they
            // differ. The tile behind the glyph stays m_color either way, as it
            // is for every other button on the toolbar.

            if (button.selected())
            {
                ImGui::PushStyleColor(ImGuiCol_Button, m_selectedColor);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, m_color);
            }
            if (ImGui::ImageButton("", (ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), m_color,
                                   m_selectedColor))
            {
                button.toggleSelected();
                if (m_onButtonClicked)
                {
                    m_onButtonClicked(button);
                }
            }
            if (ImGui::IsItemHovered())
            {
                if (m_onButtonHover)
                    m_onButtonHover(button);
            }
            ImGui::PopStyleColor(1);
        }
        else if (button.type() == OfToolbarButtonType::RadioButton)
        {

            if (button.selected())
            {
                ImGui::PushStyleColor(ImGuiCol_Button, m_selectedColor);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, m_color);
            }
            if (ImGui::ImageButton("", (ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), m_color,
                                   m_selectedColor))
            {
                this->selectButton(int(i), button.group());
                if (m_onButtonClicked)
                {
                    m_onButtonClicked(button);
                }
            }
            if (ImGui::IsItemHovered())
            {
                if (m_onButtonHover)
                    m_onButtonHover(button);
            }
            ImGui::PopStyleColor(1);
        }

        if (button.texture() != nullptr)
            button.texture()->unbind();

        anyDrawn = true;

        ImGui::PopStyleVar(1);
        ImGui::PopID();
    }
    ImGui::PopStyleColor(1);
}

OfToolbarButton::OfToolbarButton(const std::string name, OfToolbarButtonType type, ofui::TexturePtr texture, int group,
                                 int id, UiFeature feature)
    : m_name{name}, m_selected{false}, m_enabled{true}, m_group{group}, m_id{id}, m_type{type}, m_texture{texture},
      m_feature{feature}
{}

void ofui::OfToolbarButton::setSelected(bool flag)
{
    m_selected = flag;
}

bool ofui::OfToolbarButton::selected()
{
    return m_selected;
}

void ofui::OfToolbarButton::toggleSelected()
{
    m_selected = !m_selected;
}

void ofui::OfToolbarButton::select()
{
    m_selected = true;
}

void ofui::OfToolbarButton::unselect()
{
    m_selected = false;
}

void ofui::OfToolbarButton::setEnabled(bool flag)
{
    m_enabled = flag;
}

bool ofui::OfToolbarButton::enabled()
{
    return m_enabled;
}

void ofui::OfToolbarButton::enable()
{
    m_enabled = true;
}

void ofui::OfToolbarButton::disable()
{
    m_enabled = false;
}

int ofui::OfToolbarButton::group()
{
    return m_group;
}

int ofui::OfToolbarButton::id()
{
    return m_id;
}

UiFeature ofui::OfToolbarButton::feature()
{
    return m_feature;
}

ofui::TexturePtr ofui::OfToolbarButton::texture()
{
    return m_texture;
}

OfToolbarButtonType ofui::OfToolbarButton::type()
{
    return m_type;
}

std::string ofui::OfToolbarButton::name()
{
    return m_name;
}

void ofui::OfToolbarButton::setName(const std::string name)
{
    m_name = name;
}
