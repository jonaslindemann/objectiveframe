#include <ofui/result_toolbar_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

using namespace ofui;

ResultToolbarWindow::ResultToolbarWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_contentWidth{130.0f}
{
    this->setWindowFlags(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);

    this->setCanClose(false);
    this->setAutoPlacement(false);
    this->setIgnoreFocusCheck(true);
}

ResultToolbarWindow::~ResultToolbarWindow()
{}

std::shared_ptr<ResultToolbarWindow> ResultToolbarWindow::create(const std::string name)
{
    return std::make_shared<ResultToolbarWindow>(name);
}

void ResultToolbarWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

void ResultToolbarWindow::setAnchorWindow(std::shared_ptr<UiWindow> window)
{
    m_anchorWindow = window;
}

void ResultToolbarWindow::setContentWidth(float width)
{
    m_contentWidth = width;
}

float ResultToolbarWindow::contentWidth() const
{
    return m_contentWidth;
}

void ResultToolbarWindow::doPreDraw()
{
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;

    float top = work_pos.y + PAD;

    // The anchor reports absolute screen coordinates, but only after it has been
    // drawn once. Until then we fall back to the top of the work area.

    auto anchor = m_anchorWindow.lock();
    if ((anchor != nullptr) && anchor->visible() && (anchor->y() >= 0) && (anchor->height() > 0))
        top = float(anchor->y() + anchor->height()) + PAD;

    ImGui::SetNextWindowPos(ImVec2(work_pos.x + work_size.x - PAD, top), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
}

bool ResultToolbarWindow::resultButton(const char *label, int resultType, float width)
{
    bool selected = (m_view->getResultType() == resultType);

    if (selected)
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

    bool clicked = ImGui::Button(label, ImVec2(width, 0.0f));

    if (selected)
        ImGui::PopStyleColor(1);

    if (clicked)
        m_view->setResultType(resultType);

    return clicked;
}

void ResultToolbarWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    const float buttonWidth = m_contentWidth;

    this->resultButton("Normal", IVF_BEAM_N, buttonWidth);
    this->resultButton("Torsion", IVF_BEAM_T, buttonWidth);
    this->resultButton("Shear", IVF_BEAM_V, buttonWidth);
    this->resultButton("Moment", IVF_BEAM_M, buttonWidth);
    this->resultButton("Navier", IVF_BEAM_NAVIER, buttonWidth);
    this->resultButton("No result", IVF_BEAM_NO_RESULT, buttonWidth);

    ImGui::Separator();

    bool useBlending = m_view->getUseBlending();

    if (useBlending)
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

    if (ImGui::Button("X-ray mode", ImVec2(buttonWidth, 0.0f)))
        m_view->setUseBlending(!useBlending);

    if (useBlending)
        ImGui::PopStyleColor(1);
}
