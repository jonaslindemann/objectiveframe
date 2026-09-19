#include <ofui/quick_support_window.h>

#include <imgui.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

namespace {

// In ofem::BeamNodeBC::DefaultKind order, so the combo index is the kind.

const char *constraintNames = "Fixed (all dofs)\0Pinned (free to rotate)\0Roller X (free along X)\0"
                              "Roller Y (free along Y)\0Roller Z (free along Z)\0";

// What each support actually holds, so the panel says what it means rather than
// leaving the user to infer it from a name.

const char *constraintHints[5] = {
    "Holds all three translations and all three rotations.",  "Holds all three translations, free to rotate.",
    "Free to move along X, held in Y and Z. Free to rotate.", "Free to move along Y, held in X and Z. Free to rotate.",
    "Free to move along Z, held in X and Y. Free to rotate.",
};

} // namespace

QuickSupportWindow::QuickSupportWindow(const std::string name) : UiWindow(name), m_view{nullptr}
{}

QuickSupportWindow::~QuickSupportWindow()
{}

std::shared_ptr<QuickSupportWindow> QuickSupportWindow::create(const std::string name)
{
    return std::make_shared<QuickSupportWindow>(name);
}

void QuickSupportWindow::setFemView(FemViewWindow *view)
{
    m_view = view;

    pushConstraint();
}

void QuickSupportWindow::update()
{
    if (m_view == nullptr)
        return;

    m_fields.constraint = m_view->quickConstraint();
}

void QuickSupportWindow::pushConstraint()
{
    if (m_view == nullptr)
        return;

    m_view->setQuickConstraint(m_fields.constraint);
}

void QuickSupportWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    ImGui::Dummy(ImVec2(340.0f, 0.0f));

    if (ImGui::Combo("Support", &m_fields.constraint, constraintNames))
        pushConstraint();

    ImGui::TextDisabled("%s", constraintHints[m_fields.constraint]);

    ImGui::Separator();

    if (ImGui::Button("Apply to selection", ImVec2(160.0f, 0.0f)))
        m_view->quickConstraintSelection(m_fields.constraint);

    ImGui::SameLine();

    if (ImGui::Button("Remove from selection", ImVec2(180.0f, 0.0f)))
        m_view->clearQuickConstraintSelection();

    bool painting = (m_view->getEditMode() == WidgetMode::PaintBC);

    if (ImGui::Checkbox("Paint on nodes", &painting))
        m_view->setEditMode(painting ? WidgetMode::PaintBC : WidgetMode::Select);

    ImGui::TextDisabled("Drag over nodes to support them. [Ctrl] removes.");

    ImGui::Separator();

    // Worth saying out loud: every other way of assigning a BC adds one, so a
    // tool that replaces instead would otherwise be a surprise.

    ImGui::TextDisabled("A node carries one support at a time -");
    ImGui::TextDisabled("applying one replaces whatever it had.");
}
