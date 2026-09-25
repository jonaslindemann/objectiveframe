#include <ofui/quick_force_window.h>

#include <imgui.h>

#include <algorithm>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

namespace {

// The six axis directions, in button order. A quick force is axis aligned by
// design: a beginner asking for "a force" means one of these, and anything else
// is what the Node loads panel is for.
//
// Segments rather than a combo: six is few enough to show at once, the choice
// then costs one click instead of two, and which direction is armed is visible
// without opening anything.

const char *directionLabels[6] = {"-X", "+X", "-Y", "+Y", "-Z", "+Z"};

// What each direction means, on hover. Only the two vertical ones have anything
// to add - "down" is the word the user is thinking in - so the rest say nothing
// rather than padding the row with tooltips that restate their own label.

const char *directionHints[6] = {nullptr, nullptr, "Down", "Up", nullptr, nullptr};

const double directionVectors[6][3] = {
    {-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0, 1.0},
};

// The width one segment needs: the widest label in the row, so every segment is
// the same size and none of them clips. Measured rather than guessed, because
// the font scales with the interface and a constant that fits at one scale
// crops at another.

// minWidth is a floor for a row of short labels: "-X" is four characters wide
// and would make a button too small to aim at.

float segmentWidth(const char *const *labels, int count, float minWidth = 0.0f)
{
    float widest = 0.0f;

    for (int i = 0; i < count; i++)
        widest = (std::max)(widest, ImGui::CalcTextSize(labels[i]).x);

    return (std::max)(widest + 2.0f * ImGui::GetStyle().FramePadding.x, minWidth);
}

// The row those segments make up, which is also the width the rest of the panel
// lines up with.

float segmentRowWidth(const char *const *labels, int count, float minWidth = 0.0f)
{
    return count * segmentWidth(labels, count, minWidth) + (count - 1) * ImGui::GetStyle().ItemSpacing.x;
}

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
    // leaves the direction where it was, rather than snapping the direction to
    // an axis the user did not ask for.

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

    const float scale = ImGui::GetIO().FontGlobalScale;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    // Everything in the panel is as wide as the direction row, so the controls
    // stack into one block rather than three widths that nearly agree.

    const float minSegment = 30.0f * scale;
    const float segment = segmentWidth(directionLabels, 6, minSegment);
    const float rowWidth = segmentRowWidth(directionLabels, 6, minSegment);

    ImGui::Dummy(ImVec2(rowWidth, 0.0f));

    ImGui::SetNextItemWidth(rowWidth);

    if (ImGui::InputDouble("Magnitude", &m_fields.magnitude, 0.0, 0.0, "%.6g"))
        pushForce();

    for (int i = 0; i < 6; i++)
    {
        if (i > 0)
            ImGui::SameLine();

        const bool selected = (m_fields.direction == i);

        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

        ImGui::PushID(i);

        if (ImGui::Button(directionLabels[i], ImVec2(segment, 0.0f)))
        {
            m_fields.direction = i;
            pushForce();
        }

        ImGui::PopID();

        if (selected)
            ImGui::PopStyleColor(1);

        if ((directionHints[i] != nullptr) && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", directionHints[i]);
    }

    ImGui::SameLine();
    ImGui::TextUnformatted("Direction");

    ImGui::Separator();

    // The Apply button says what it will act on, which is why there is no line
    // of text under it saying the same thing. With nothing selected it becomes
    // the instruction: the only two ways to use the tool are on this row.

    const int selectedNodes = m_view->selectedNodeCount();
    const bool haveSelection = (selectedNodes > 0);

    std::string applyLabel =
        haveSelection ? ("Apply to " + std::to_string(selectedNodes) + " nodes") : std::string("Apply to selection");

    // Sized off the row above rather than off the label, so the panel does not
    // change width every time the selection does.

    const float removeWidth = ImGui::CalcTextSize("Remove").x + 2.0f * ImGui::GetStyle().FramePadding.x + 8.0f * scale;
    const float applyWidth = rowWidth - spacing - removeWidth;

    ImGui::BeginDisabled(!haveSelection);

    if (ImGui::Button(applyLabel.c_str(), ImVec2(applyWidth, 0.0f)))
    {
        double fx, fy, fz;
        m_view->quickForce(fx, fy, fz);
        m_view->quickForceSelection(fx, fy, fz);
    }

    ImGui::EndDisabled();

    // Which load the next click feeds. It matters - nodes loaded the same way
    // share one load, and that name is what the user looks for in the load list
    // and the load mixer - but not enough to hold a line of the panel, so it
    // hangs off the button it describes. Outside the BeginDisabled pair: a
    // disabled item reports no hover, and the answer is worth having precisely
    // when there is no selection yet.

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("Creates or joins: %s\nNodes loaded the same way share one load.",
                          m_view->quickForceName().c_str());

    ImGui::SameLine();

    ImGui::BeginDisabled(!haveSelection);

    if (ImGui::Button("Remove", ImVec2(removeWidth, 0.0f)))
        m_view->clearQuickForceSelection();

    ImGui::EndDisabled();

    ImGui::TextDisabled("Drag over nodes to paint. [Ctrl] removes.");
}
