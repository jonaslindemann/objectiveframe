#include <ofui/start_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <ofutil/util_functions.h>

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

    // ImGui::Dummy(ImVec2(800.0, 800.0));
    ImGui::Text("Welcome to ObjectiveFrame!");
    ImGui::NewLine();

    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 45.0f);
    ImGui::TextUnformatted(
        "Select one of the examples below or use the buttons to the right to create new models or open "
        "existing models.");
    ImGui::PopTextWrapPos();
    ImGui::Dummy(ImVec2(0.0, 20.0f * scale));

    auto imageIdx = 0;

    if (ImGui::BeginTable("table2", 2))
        ;
    {
        ImGui::TableSetupColumn("Example", ImGuiTableColumnFlags_WidthFixed, 800.0f * scale);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 200.0f * scale);
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
                                               ImVec2(180.0f * scale, 180.0f * scale)))
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
        ImGui::TableNextColumn();
        if (ImGui::Button("New Project...", ImVec2(180.0f * scale, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::NewModel;
                m_startButtonClickedFunc(button);
            }
        }
        if (ImGui::Button("Open Model...", ImVec2(180.0f * scale, 0)))
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

        if (ImGui::Button("Open AI prompt...", ImVec2(180.0f * scale, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenAIPrompt;
                m_startButtonClickedFunc(button);
            }
        }

        if (ImGui::Button("Open Python model...", ImVec2(180.0f * scale, 0)))
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

        ImGui::Dummy(ImVec2(0.0, 20.0f * scale));

        // The interface profile, offered here because this is where a new user
        // arrives before anything is on screen to be confused by. It is read
        // straight off the profile rather than mirrored into a member, so it
        // stays right when the mode is changed from the menu instead.
        //
        // Applied on the click rather than when the dialog closes: the toolbars
        // are drawn behind this window, so the change is visible immediately
        // and can be reconsidered without leaving the page.

        // The explanation sits in a tooltip rather than under the buttons so
        // that this block is the same height in both modes - the popup is
        // AlwaysAutoResize, and a description that grew when Simple was picked
        // would resize the window under the pointer that just picked it.

        ImGui::TextUnformatted("Interface");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Simple hides beam types, the load, support and material dialogs, and the "
                              "selection filters.\nModels keep all of their data either way, and the mode "
                              "can be changed at any time.");

        auto *profile = UiProfile::instance();
        auto currentMode = profile->mode();

        if (ImGui::RadioButton("Simple", currentMode == UiMode::Simple))
        {
            if (m_uiModeChangedFunc)
                m_uiModeChangedFunc(UiMode::Simple);
        }

        if (ImGui::RadioButton("Advanced", currentMode == UiMode::Advanced))
        {
            if (m_uiModeChangedFunc)
                m_uiModeChangedFunc(UiMode::Advanced);
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * scale));

        if (ImGui::Button("Documentation", ImVec2(180.0f * scale, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::Documentation;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * scale));

        /*

        if (ImGui::Button("Start automation API", ImVec2(180.0f * scale, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::StartAPI;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0f * scale));
        */

        if (ImGui::Button("Close", ImVec2(180.0f * scale, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
        }

        // Pads the column down so the version string lands level with the
        // bottom of the example grid. Reduced from 530 by the height of the
        // interface block added above.

        ImGui::Dummy(ImVec2(0.0, 440.0f * scale));

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 45.0f);
        ImGui::TextUnformatted(m_versionString.c_str());
        ImGui::TextUnformatted(m_release.c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndTable();
    }
}
