#include <ofui/array_grid_popup.h>

#include <string>

#include <imgui.h>
#include <imgui_internal.h>

#include <ofui/plane_axes.h>

using namespace ofui;

ArrayGridPopup::ArrayGridPopup(const std::string name, bool modal)
    : PopupWindow(name, modal), m_plane{1}, m_count1{4}, m_count2{3}, m_step1{5.0f}, m_step2{5.0f}, m_spanStep{false},
      m_copyLoads{true}, m_weldTolerance{0.001f}
{}

ArrayGridPopup::~ArrayGridPopup()
{}

std::shared_ptr<ArrayGridPopup> ArrayGridPopup::create(const std::string name, bool modal)
{
    return std::make_shared<ArrayGridPopup>(name, modal);
}

int ArrayGridPopup::plane()
{
    return m_plane;
}

int ArrayGridPopup::count1()
{
    return m_count1;
}

int ArrayGridPopup::count2()
{
    return m_count2;
}

float ArrayGridPopup::step1()
{
    return m_step1;
}

float ArrayGridPopup::step2()
{
    return m_step2;
}

bool ArrayGridPopup::spanStep()
{
    return m_spanStep;
}

bool ArrayGridPopup::copyLoads()
{
    return m_copyLoads;
}

float ArrayGridPopup::weldTolerance()
{
    return m_weldTolerance;
}

void ArrayGridPopup::plane(int value)
{
    m_plane = value;
}

void ArrayGridPopup::count1(int value)
{
    m_count1 = value;
}

void ArrayGridPopup::count2(int value)
{
    m_count2 = value;
}

void ArrayGridPopup::step1(float value)
{
    m_step1 = value;
}

void ArrayGridPopup::step2(float value)
{
    m_step2 = value;
}

void ArrayGridPopup::spanStep(bool value)
{
    m_spanStep = value;
}

void ArrayGridPopup::copyLoads(bool value)
{
    m_copyLoads = value;
}

void ArrayGridPopup::weldTolerance(float value)
{
    m_weldTolerance = value;
}

void ArrayGridPopup::doPopup()
{
    const std::string axis1 = planeAxisLabel(m_plane, 0);
    const std::string axis2 = planeAxisLabel(m_plane, 1);

    ImGui::Text("Repeat the selection across the %s%s plane.", axis1.c_str(), axis2.c_str());

    ImGui::Separator();

    ImGui::DragInt((axis1 + " repeat").c_str(), &m_count1, 0.2f, 1, 100);
    ImGui::DragFloat((axis1 + " step").c_str(), &m_step1, 0.05f);

    ImGui::DragInt((axis2 + " repeat").c_str(), &m_count2, 0.2f, 1, 100);
    ImGui::DragFloat((axis2 + " step").c_str(), &m_step2, 0.05f);

    ImGui::Separator();

    ImGui::Checkbox("Steps in selection lengths", &m_spanStep);

    if (m_spanStep)
        ImGui::TextUnformatted("A step of 1.0 is one bounding box length of the\n"
                               "selection. A flat selection has no length across\n"
                               "its plane - that step would come out zero.");

    ImGui::Checkbox("Copy loads and boundary conditions", &m_copyLoads);
    ImGui::DragFloat("Weld tolerance", &m_weldTolerance, 0.0005f, 0.0f, 1.0f, "%.4f");

    // The repeats counting the original is the one thing people get wrong, so
    // show what the numbers currently in the fields will actually produce.

    const int instances = m_count1 * m_count2;

    ImGui::Separator();
    ImGui::Text("Repeats include the original: %d x %d = %d instances, %d copies.", m_count1, m_count2, instances,
                (instances > 0) ? instances - 1 : 0);

    ImVec2 button_size = ImGui::CalcItemSize(ImVec2{120, 0}, 0.0f, 0.0f);
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 centre_position_for_button{(winSize.x - button_size.x * 2 - ImGui::GetStyle().ItemSpacing.x) / 2,
                                      (winSize.y - button_size.y) / 2};

    ImGui::NewLine();

    ImGui::SetCursorPosX(centre_position_for_button.x);
    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        this->close(PopupResult::OK);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0)))
    {
        this->close(PopupResult::CANCEL);
        ImGui::CloseCurrentPopup();
    }
}
