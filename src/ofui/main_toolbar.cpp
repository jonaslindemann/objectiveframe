#include <ofui/main_toolbar.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

using namespace ofui;

MainToolbar::MainToolbar(const std::string name) : UiWindow(name), m_view{nullptr}, m_selectedButton{-1}
{
    setWindowFlags(ImGuiWindowFlags_NoCollapse);

    m_texture = Texture::create("D:\\Users\\Jonas\\Development\\objectiveframe\\bin\\Debug\\images\\tlviewpan.png");
    m_texture->load();

    this->addButton("Box", OfToolbarButtonType::Button, 0, 0);
    this->addButton("Sphere", OfToolbarButtonType::Button, 0, 1);
    this->addButton("Cylinder", OfToolbarButtonType::Button, 0, 2);

    this->addButton("Cone", OfToolbarButtonType::ToggleButton, 0, 3);

    this->addButton("Torus", OfToolbarButtonType::RadioButton, 1, 4);
    this->addButton("Torus Knot", OfToolbarButtonType::RadioButton, 1, 5);
    this->addButton("Plane", OfToolbarButtonType::RadioButton, 1, 6);
}

MainToolbar::~MainToolbar()
{
}
#ifdef USE_FEMVIEW
void ofui::MainToolbar::setView(FemViewWindow *view)
{
    m_view = view;
}
#else
void MainToolbar::setWidget(FemWidget *femWidget)
{
    m_view = femWidget;
    m_scaleFactor = float(m_view->getScalefactor());
    m_lockScaleFactor = m_view->isScaleFactorLocked();
}
#endif

void ofui::MainToolbar::addButton(const std::string name, OfToolbarButtonType type, int group, int id)
{
    m_buttons.emplace_back(name, type, group, id);
}
ofui::OfToolbarButton &ofui::MainToolbar::button(int idx)
{
    return m_buttons[idx];
}

size_t ofui::MainToolbar::buttonCount()
{
    return m_buttons.size();
}

void MainToolbar::update()
{
}

std::shared_ptr<MainToolbar> MainToolbar::create(const std::string name)
{
    return std::make_shared<MainToolbar>(name);
}

void MainToolbar::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    ImVec2 button_sz(40, 40);

    ImGuiStyle &style = ImGui::GetStyle();
    int buttons_count = 20;
    float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    int id = 0;

    m_texture->bind();

    for (auto &button : m_buttons) {
        ImGui::PushID(id++);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        if (button.type() == OfToolbarButtonType::Button) {
            if (ImGui::ImageButton((ImTextureID)(m_texture->id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4)) {
            }
        }
        else if (button.type() == OfToolbarButtonType::ToggleButton) {
            if (button.selected()) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 1, 1});
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0, 0, 0.5, 1.0});
            }
            if (ImGui::ImageButton((ImTextureID)(m_texture->id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4)) {
                button.toggleSelected();
            }
            ImGui::PopStyleColor(1);
        }
        else if (button.type() == OfToolbarButtonType::RadioButton) {
            if (ImGui::ImageButton((ImTextureID)(m_texture->id()), button_sz, ImVec2(0, 0), ImVec2(1, 1), 4,
                                   ImVec4{1, 0, 0, 1}, ImVec4{0, 2, 0, 1})) {
            }
        }

        m_texture->unbind();

        ImGui::PopStyleVar(1);
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 =
            last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
        if (id < buttons_count && next_button_x2 < window_visible_x2)
            ImGui::SameLine();
        ImGui::PopID();
    }
}

/*
for (int n = 0; n < buttons_count; n++) {
    if (n == m_selectedButton) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.1f, 0.1f, 1.0f});
    }
    ImGui::PushID(n);
    // ImGui::ImageButton(0, button_sz, ImVec2(0, 0), ImVec2(1, 1), 2, );
    if (ImGui::Button("Box", button_sz)) {
        m_selectedButton = n;
    }
    float last_button_x2 = ImGui::GetItemRectMax().x;
    float next_button_x2 =
        last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
    if (n + 1 < buttons_count && next_button_x2 < window_visible_x2)
        ImGui::SameLine();
    ImGui::PopID();
    ImGui::PopStyleColor(1);
}
*/

/*
static bool pressed = true;

if (pressed)
    ImGui::PushStyleColor(ImGuiCol_Button, color);
else
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});
bool temp = ImGui::Button(buttonName);

if (temp == true)
{
   pressed = 1 - pressed;
}

ImGui::PopStyleColor(1);

if (m_view != nullptr) {
}
}
*/

OfToolbarButton::OfToolbarButton(const std::string name, OfToolbarButtonType type, int group, int id)
    : m_name{name}, m_selected{false}, m_enabled{true}, m_group{group}, m_id{id}, m_type{type}
{
}

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
