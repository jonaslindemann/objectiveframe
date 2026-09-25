#include <ofui/quick_support_window.h>

#include <imgui.h>

#include <algorithm>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <string>

using namespace ofui;

namespace {

// In ofem::BeamNodeBC::DefaultKind order, so the index is the kind.
//
// Short labels: the row has to fit five of them, and what each one holds is on
// hover rather than in the label and then again underneath it.

const char *constraintLabels[5] = {"Fixed", "Pinned", "Roll X", "Roll Y", "Roll Z"};

// What each support actually holds, on hover, so the panel can say what it
// means without a sentence of it standing on screen at all times.

const char *constraintHints[5] = {
    "Holds all three translations and all three rotations.",  "Holds all three translations, free to rotate.",
    "Free to move along X, held in Y and Z. Free to rotate.", "Free to move along Y, held in X and Z. Free to rotate.",
    "Free to move along Z, held in X and Y. Free to rotate.",
};

// The width one segment needs: the widest label in the row, so every segment is
// the same size and none of them clips. Measured rather than guessed, because
// the font scales with the interface and a constant that fits at one scale
// crops at another.

float segmentWidth(const char *const *labels, int count)
{
    float widest = 0.0f;

    for (int i = 0; i < count; i++)
        widest = (std::max)(widest, ImGui::CalcTextSize(labels[i]).x);

    return widest + 2.0f * ImGui::GetStyle().FramePadding.x;
}

// The row those segments make up, which is also the width the rest of the panel
// lines up with.

float segmentRowWidth(const char *const *labels, int count)
{
    return count * segmentWidth(labels, count) + (count - 1) * ImGui::GetStyle().ItemSpacing.x;
}

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

    const float scale = ImGui::GetIO().FontGlobalScale;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    // Segments rather than a combo, as in the force panel: five is few enough
    // to show at once, and which support is armed is then visible without
    // opening anything. The action row below is sized to match, so the panel is
    // one block rather than two widths that nearly agree.

    const float segment = segmentWidth(constraintLabels, 5);
    const float rowWidth = segmentRowWidth(constraintLabels, 5);

    ImGui::Dummy(ImVec2(rowWidth, 0.0f));

    for (int i = 0; i < 5; i++)
    {
        if (i > 0)
            ImGui::SameLine();

        const bool selected = (m_fields.constraint == i);

        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

        ImGui::PushID(i);

        if (ImGui::Button(constraintLabels[i], ImVec2(segment, 0.0f)))
        {
            m_fields.constraint = i;
            pushConstraint();
        }

        ImGui::PopID();

        if (selected)
            ImGui::PopStyleColor(1);

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", constraintHints[i]);
    }

    ImGui::Separator();

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
        m_view->quickConstraintSelection(m_fields.constraint);

    ImGui::EndDisabled();

    // Worth saying, because every other way of assigning a BC adds one and a
    // tool that replaces instead would be a surprise - but on the button it
    // describes rather than as two lines standing at the foot of the panel.
    // Asked outside the BeginDisabled pair: a disabled item reports no hover.

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("A node carries one support at a time.\nApplying one replaces whatever it had.");

    ImGui::SameLine();

    ImGui::BeginDisabled(!haveSelection);

    if (ImGui::Button("Remove", ImVec2(removeWidth, 0.0f)))
        m_view->clearQuickConstraintSelection();

    ImGui::EndDisabled();

    ImGui::TextDisabled("Drag over nodes to paint. [Ctrl] removes.");
}
