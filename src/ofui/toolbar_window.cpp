#include <ofui/toolbar_window.h>

#include <vfem/beam_model.h>

using namespace ofui;

ToolbarWindow::ToolbarWindow(const std::string name)
    : UiWindow(name), m_selectedButton{-1}, m_selectedColor{0.0, 0.0, 1.0, 1.0}, m_color{0.0, 0.0, 0.0, 1.0}
{
    setWindowFlags(ImGuiWindowFlags_NoCollapse);
    setCanClose(false);
    setAutoPlacement(false);

    m_color = ImGui::GetStyle().Colors[ImGuiCol_Button];
    m_selectedColor = {1.0, 1.0, 1.0, 1.0};
}

ToolbarWindow::~ToolbarWindow()
{}

void ofui::ToolbarWindow::addButton(const std::string name, OfToolbarButtonType type, std::string filename, int group)
{
    auto texture = Texture::create(filename);
    texture->load();
    m_buttons.emplace_back(name, type, texture, group, texture->id());
}
void ofui::ToolbarWindow::addSpacer()
{
    m_buttons.emplace_back("", OfToolbarButtonType::Spacer, nullptr, -1, -1);
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

    ImVec2 button_sz(40, 40);

    ImGuiStyle &style = ImGui::GetStyle();
    int buttons_count = 20;
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    int id = 0;

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4{1, 0, 0, 1});

    for (auto &button : m_buttons)
    {
        ImGui::PushID(id++);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});

        if (button.texture() != nullptr)
            button.texture()->bind();

        if (button.type() == OfToolbarButtonType::Button)
        {
            if (ImGui::ImageButton((ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4, m_color,
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
            if (button.selected())
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 1, 1});
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0.5, 1.0});
            }
            if (ImGui::ImageButton((ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4, m_color,
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
            if (ImGui::ImageButton((ImTextureID)(button.id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4, m_color,
                                   m_selectedColor))
            {
                this->selectButton(id - 1, button.group());
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
        else if (button.type() == OfToolbarButtonType::Spacer)
        {
            ImGui::Dummy(ImVec2(10, 10));
        }

        if (button.texture() != nullptr)
            button.texture()->unbind();

        ImGui::PopStyleVar(1);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 =
            last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
        if (id < buttons_count && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
    ImGui::PopStyleColor(1);
}

OfToolbarButton::OfToolbarButton(const std::string name, OfToolbarButtonType type, ofui::TexturePtr texture, int group,
                                 int id)
    : m_name{name}, m_selected{false}, m_enabled{true}, m_group{group}, m_id{id}, m_type{type}, m_texture{texture}
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
