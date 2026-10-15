#include <ofui/start_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <ofutil/util_functions.h>

#include <algorithm>
#include <cmath>
#include <format>

using namespace ofui;

StartPopup::StartPopup(const std::string name, bool modal)
    : PopupWindow(name, modal), m_view(nullptr), m_startButtonClickedFunc(nullptr), m_exampleClickedFunc(nullptr),
      m_uiModeChangedFunc(nullptr)
{
    this->setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_AlwaysAutoResize);
}

StartPopup::~StartPopup()
{}

std::shared_ptr<StartPopup> StartPopup::create(const std::string name, bool modal)
{
    return std::make_shared<StartPopup>(name, modal);
}

void ofui::StartPopup::setView(FemViewWindow *view)
{
    m_view = view;
}

void ofui::StartPopup::addExample(const std::string &filename, const std::string &imageFilename)
{
    m_exampleFilename.push_back(filename);

    auto texture = Texture::create(imageFilename);
    texture->load();

    m_exampleImage.push_back(texture);
}

void ofui::StartPopup::assignStartButtonClickedFunc(StartButtonClickedFunc func)
{
    m_startButtonClickedFunc = func;
}

void ofui::StartPopup::assignExampleClickedFunc(ExampleClickedFunc func)
{
    m_exampleClickedFunc = func;
}

void ofui::StartPopup::assignUiModeChangedFunc(UiModeChangedFunc func)
{
    m_uiModeChangedFunc = func;
}

void ofui::StartPopup::setVersionString(const std::string &versionString)
{
    m_versionString = versionString;
}

void ofui::StartPopup::setRelease(const std::string &release)
{
    m_release = release;
}

void StartPopup::doPopup()
{
    const float scale = ImGui::GetIO().FontGlobalScale;

    // The example grid, its column and the vertical spacers shrink with m_fit
    // so the popup fits the window; buttons and text keep their size.
    const float fit = scale * m_fit;

    // The thumbnails and the buttons decide the width of the popup; the text
    // above wraps to it rather than widening the window.
    //
    // The example column holds four thumbnails with their frame padding and
    // the cell gaps between them - padding that does not shrink with them. The
    // actions column is exactly as wide as its widest item and the buttons
    // fill it, so the right edge sits on the window padding just as the left
    // edge does: tables pad between columns, not along their outer edges.
    const ImGuiStyle &style = ImGui::GetStyle();
    const float cellGap = 2.0f * style.CellPadding.x;
    const float exampleWidth = 4.0f * (180.0f * fit + 2.0f * style.FramePadding.x) + 3.0f * cellGap;

    const float radioWidth = ImGui::GetFrameHeight() + style.ItemInnerSpacing.x;
    const float actionsWidth = (std::max)({180.0f * scale, ImGui::CalcTextSize(m_versionString.c_str()).x,
                                          ImGui::CalcTextSize(m_release.c_str()).x,
                                          radioWidth + ImGui::CalcTextSize("Simple - Beam").x});
    const float contentWidth = exampleWidth + cellGap + actionsWidth;

    ImGui::Text("Welcome to ObjectiveFrame!");
    ImGui::NewLine();

    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + contentWidth);
    ImGui::TextUnformatted(
        "Select one of the examples below or use the buttons to the right to create new models or open "
        "existing models.");
    ImGui::PopTextWrapPos();
    ImGui::Dummy(ImVec2(0.0, 20.0f * fit));

    auto imageIdx = 0;

    // An explicit outer width: a table left to fill the available width would
    // hold the auto-resizing window at whatever width it had last frame.
    if (ImGui::BeginTable("table2", 2, ImGuiTableFlags_SizingFixedFit, ImVec2(contentWidth, 0.0f)))
    {
        ImGui::TableSetupColumn("Example", ImGuiTableColumnFlags_WidthFixed, exampleWidth);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, actionsWidth);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (ImGui::BeginTable("table3", 4, ImGuiTableFlags_SizingFixedFit))
        {
            for (int i = 0; i < 4; i++)
            {
                ImGui::TableNextRow();
                for (int j = 0; j < 4; j++)
                {
                    ImGui::TableNextColumn();
                    if (imageIdx < m_exampleImage.size())
                    {
                        m_exampleImage[imageIdx]->bind();
                        auto id = "##" + ofutil::to_string(i) + ofutil::to_string(j);
                        if (ImGui::ImageButton(id.c_str(), (ImTextureID)m_exampleImage[imageIdx]->id(),
                                               ImVec2(180.0f * fit, 180.0f * fit)))
                        {
                            if (m_exampleClickedFunc)
                                m_exampleClickedFunc(m_exampleFilename[imageIdx]);
                            ImGui::CloseCurrentPopup();
                            this->close(PopupResult::OK);
                        }
                        m_exampleImage[imageIdx]->unbind();
                        imageIdx++;
                    }
                    else
                    {
                        // ImGui::Text("No image");
                    }
                }
            }
            ImGui::EndTable();
        }

        // Where the example grid ends, so the version text can be lined up
        // with it at the bottom of the actions column.
        const float gridBottom = ImGui::GetItemRectMax().y;

        ImGui::TableNextColumn();
        if (ImGui::Button("New Project...", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::NewModel;
                m_startButtonClickedFunc(button);
            }
        }
        if (ImGui::Button("Open Model...", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenModel;
                m_startButtonClickedFunc(button);
            }
        }

        /*

        if (ImGui::Button("Open AI prompt...", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenAIPrompt;
                m_startButtonClickedFunc(button);
            }
        }

        if (ImGui::Button("Open Python model...", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenPythonModel;
                m_startButtonClickedFunc(button);
            }
        }
        */

        ImGui::Dummy(ImVec2(0.0, 20.0f * fit));

        // The interface profile, offered here because this is where a new user
        // arrives before anything is on screen to be confused by. It is read
        // straight off the profile rather than mirrored into a member, so it
        // stays right when the mode is changed from the menu instead.
        //
        // Applied on the click rather than when the dialog closes: the toolbars
        // are drawn behind this window, so the change is visible immediately
        // and can be reconsidered without leaving the page.

        // The explanation sits in a tooltip rather than under the buttons so
        // that this block is the same height in every mode - the popup is
        // AlwaysAutoResize, and a description that grew when Simple was picked
        // would resize the window under the pointer that just picked it.

        ImGui::TextUnformatted("Interface");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The simple modes hide the load, support and material dialogs and the "
                              "selection filters, and build from one kind of element.\nBar structures "
                              "carry axial force alone, so that mode shows normal force only; beam "
                              "structures get the full set of sectional results.\nModels keep all of "
                              "their data whichever mode is in force, and the mode can be changed at any "
                              "time.");

        auto *profile = UiProfile::instance();
        auto currentMode = profile->mode();

        if (ImGui::RadioButton("Simple - Bar", currentMode == UiMode::SimpleBar))
        {
            if (m_uiModeChangedFunc)
                m_uiModeChangedFunc(UiMode::SimpleBar);
        }

        if (ImGui::RadioButton("Simple - Beam", currentMode == UiMode::SimpleBeam))
        {
            if (m_uiModeChangedFunc)
                m_uiModeChangedFunc(UiMode::SimpleBeam);
        }

        if (ImGui::RadioButton("Advanced", currentMode == UiMode::Advanced))
        {
            if (m_uiModeChangedFunc)
                m_uiModeChangedFunc(UiMode::Advanced);
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * fit));

        if (ImGui::Button("Documentation", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::Documentation;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * fit));

        /*

        if (ImGui::Button("Start automation API", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::StartAPI;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * fit));
        */

        if (ImGui::Button("Close", ImVec2(-FLT_MIN, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
        }

        // Pad the column down so the two version lines end level with the
        // bottom of the example grid. Nothing is added when the column is
        // already the taller of the two, so the popup is never taller than its
        // content needs.

        const float versionHeight = 2.0f * ImGui::GetTextLineHeightWithSpacing() - style.ItemSpacing.y;
        const float padding = gridBottom - ImGui::GetCursorScreenPos().y - versionHeight;
        if (padding > 0.0f)
            ImGui::Dummy(ImVec2(0.0f, padding - style.ItemSpacing.y));

        ImGui::TextUnformatted(m_versionString.c_str());
        ImGui::TextUnformatted(m_release.c_str());

        ImGui::EndTable();
    }

    this->fitToViewport();
}

void StartPopup::fitToViewport()
{
    // Keep the popup within 90% of the window's work area. The grid scales
    // with m_fit but the text does not, so one correction cannot hit the
    // target exactly - it converges over a few corrections instead. An
    // auto-resizing window takes its size from the previous frame's content,
    // so after each change wait for the new size to show up before measuring
    // again; measuring straight away would correct twice and overshoot.

    if (m_settleFrames > 0)
    {
        m_settleFrames--;
        return;
    }

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    const ImVec2 size = ImGui::GetWindowSize();
    if (size.x <= 0.0f || size.y <= 0.0f)
        return;

    const float ratio =
        (std::min)(0.9f * viewport->WorkSize.x / size.x, 0.9f * viewport->WorkSize.y / size.y);
    const float target = std::clamp(m_fit * ratio, 0.3f, 1.0f);

    if (std::abs(target - m_fit) > 0.01f)
    {
        m_fit = target;
        m_settleFrames = 2;
        m_recenter = true;
    }
    else if (m_recenter)
    {
        // Centred once on appearing; re-centre at the settled size.
        ImGui::SetWindowPos(ImVec2(viewport->WorkPos.x + (viewport->WorkSize.x - size.x) * 0.5f,
                                   viewport->WorkPos.y + (viewport->WorkSize.y - size.y) * 0.5f));
        m_recenter = false;
    }
}
