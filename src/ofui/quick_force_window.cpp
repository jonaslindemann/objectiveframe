#include <ofui/quick_force_window.h>

#include <imgui.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

namespace {

// The six axis directions, in combo order. A quick force is axis aligned by
// design: a beginner asking for "a force" means one of these, and anything else
// is what the Node loads panel is for.

const char *directionNames = "-X\0+X\0-Y (down)\0+Y (up)\0-Z\0+Z\0";

const double directionVectors[6][3] = {
    {-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0, 1.0},
};

} // namespace

QuickForceWindow::QuickForceWindow(const std::string name) : UiWindow(name), m_view{nullptr}
{}

QuickForceWindow::~QuickForceWindow()
{}

std::shared_ptr<QuickForceWindow> QuickForceWindow::create(const std::string name)
{
    return std::make_shared<QuickForceWindow>(name);
}

void QuickForceWindow::setFemView(FemViewWindow *view)
{
    m_view = view;

    // The view starts with its own default force; push the panel's so the two
    // agree before the user has touched anything.

    pushForce();
}

void QuickForceWindow::update()
{
    if (m_view == nullptr)
        return;

    double fx, fy, fz;
    m_view->quickForce(fx, fy, fz);

    double magnitude = std::sqrt(fx * fx + fy * fy + fz * fz);

    if (magnitude < 1.0e-12)
        return;

    m_fields.magnitude = magnitude;

    // Only axis aligned forces have a direction this panel can show. A force
    // set from a script along some other direction keeps its magnitude here and
    // leaves the direction combo where it was, rather than snapping the
    // direction to an axis the user did not ask for.

    for (int i = 0; i < 6; i++)
    {
        if (std::abs(fx - directionVectors[i][0] * magnitude) > 1.0e-9)
            continue;
        if (std::abs(fy - directionVectors[i][1] * magnitude) > 1.0e-9)
            continue;
        if (std::abs(fz - directionVectors[i][2] * magnitude) > 1.0e-9)
            continue;

        m_fields.direction = i;
        break;
    }
}

void QuickForceWindow::pushForce()
{
    if (m_view == nullptr)
        return;

    const double *direction = directionVectors[m_fields.direction];

    m_view->setQuickForce(direction[0] * m_fields.magnitude, direction[1] * m_fields.magnitude,
                          direction[2] * m_fields.magnitude);
}

void QuickForceWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    ImGui::Dummy(ImVec2(340.0f, 0.0f));

    if (ImGui::InputDouble("Magnitude", &m_fields.magnitude, 0.0, 0.0, "%.6g"))
        pushForce();

    if (ImGui::Combo("Direction", &m_fields.direction, directionNames))
        pushForce();

    ImGui::Separator();

    // Named here rather than only in the load list, because the name is what
    // the user will look for in the load list and the load mixer afterwards.
    // Nodes loaded the same way share one load, and so share this name.

    ImGui::Text("Creates or joins: %s", m_view->quickForceName().c_str());

    ImGui::Separator();

    double fx, fy, fz;
    m_view->quickForce(fx, fy, fz);

    if (ImGui::Button("Apply to selection", ImVec2(160.0f, 0.0f)))
        m_view->quickForceSelection(fx, fy, fz);

    ImGui::SameLine();

    if (ImGui::Button("Remove from selection", ImVec2(180.0f, 0.0f)))
        m_view->clearQuickForceSelection();

    // Reads the mode rather than remembering a flag of its own, so the checkbox
    // and the toolbar button cannot disagree about whether painting is on.

    bool painting = (m_view->getEditMode() == WidgetMode::PaintLoad);

    if (ImGui::Checkbox("Paint on nodes", &painting))
        m_view->setEditMode(painting ? WidgetMode::PaintLoad : WidgetMode::Select);

    ImGui::TextDisabled("Drag over nodes to load them. [Ctrl] removes.");
}
