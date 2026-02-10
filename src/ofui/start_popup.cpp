#include <ofui/start_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <ofutil/util_functions.h>

#include <format>

using namespace ofui;

StartPopup::StartPopup(const std::string name, bool modal)
    : PopupWindow(name, modal), m_view(nullptr), m_startButtonClickedFunc(nullptr), m_exampleClickedFunc(nullptr)
{
    this->setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    // this->setSize(1000, -1);
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
    // ImGui::Dummy(ImVec2(800.0, 800.0));
    ImGui::Text("Welcome to ObjectiveFrame!");
    ImGui::NewLine();

    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 45.0f);
    ImGui::TextUnformatted(
        "Select one of the examples below or use the buttons to the right to create new models or open "
        "existing models.");
    ImGui::PopTextWrapPos();
    ImGui::Dummy(ImVec2(0.0, 20.0));

    auto imageIdx = 0;

    if (ImGui::BeginTable("table2", 2))
        ;
    {
        ImGui::TableSetupColumn("Example", ImGuiTableColumnFlags_WidthFixed, 800);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 200);
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
                                               ImVec2(180, 180)))
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
        if (ImGui::Button("New Project...", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::NewModel;
                m_startButtonClickedFunc(button);
            }
        }
        if (ImGui::Button("Open Model...", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenModel;
                m_startButtonClickedFunc(button);
            }
        }

        if (ImGui::Button("Open AI prompt...", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenAIPrompt;
                m_startButtonClickedFunc(button);
            }
        }

        if (ImGui::Button("Open Python model...", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::OpenPythonModel;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0));

        if (ImGui::Button("Documentation", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::Documentation;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0));

        if (ImGui::Button("Start automation API", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
            if (m_startButtonClickedFunc)
            {
                OfStartButton button = OfStartButton::StartAPI;
                m_startButtonClickedFunc(button);
            }
        }

        ImGui::Dummy(ImVec2(0.0, 20.0));

        if (ImGui::Button("Close", ImVec2(180, 0)))
        {
            this->close(PopupResult::OK);
            ImGui::CloseCurrentPopup();
        }

        ImGui::Dummy(ImVec2(0.0, 430.0));

        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 45.0f);
        ImGui::TextUnformatted(m_versionString.c_str());
        ImGui::TextUnformatted(m_release.c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndTable();
    }
}
