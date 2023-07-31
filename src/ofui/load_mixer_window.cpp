#include <ofui/load_mixer_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

#include <algorithm>

using namespace ofui;

LoadMixerWindow::LoadMixerWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_femNodeLoadSet{nullptr}, m_loadSum{0.0}, m_lastSum{0.0}, m_firstUpdate{true}
{
    std::fill(std::begin(m_loadScaleFactors), std::begin(m_loadScaleFactors) + 255, 1.0f);
    m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::begin(m_loadScaleFactors) + 255);
}

LoadMixerWindow::~LoadMixerWindow()
{
}

void ofui::LoadMixerWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

void ofui::LoadMixerWindow::setFemNodeLoadSet(ofem::BeamNodeLoadSet *bcSet)
{
    m_femNodeLoadSet = bcSet;
    if (m_view != nullptr)
        m_view->setNeedRecalc(true);
}

void LoadMixerWindow::update()
{
    if (m_view != nullptr) {
        if (m_femNodeLoadSet != nullptr) {
            for (auto i = 0; i < m_femNodeLoadSet->getSize(); i++) {
                auto nodeLoad = static_cast<ofem::BeamNodeLoad *>(m_femNodeLoadSet->getLoad(i));
                nodeLoad->setScale(m_loadScaleFactors[i]);
            }

            if (m_loadSum != m_lastSum) {
                m_view->recompute();
            }

            m_firstUpdate = false;

            m_lastSum = m_loadSum;
        }
    }
}

std::shared_ptr<LoadMixerWindow> LoadMixerWindow::create(const std::string name)
{
    return std::make_shared<LoadMixerWindow>(name);
}

void LoadMixerWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    if (m_view != nullptr) {
        if (m_femNodeLoadSet != nullptr) {
            if (m_femNodeLoadSet->getSize() > 0) {
                for (auto i = 0; i < m_femNodeLoadSet->getSize(); i++) {
                    auto nodeLoad = static_cast<ofem::BeamNodeLoad *>(m_femNodeLoadSet->getLoad(i));

                    ImGui::SliderFloat(nodeLoad->getName().c_str(), &m_loadScaleFactors[i], -10.0, 10.0);
                }

                ImGui::Separator();

                if (ImGui::Button("Reset")) {
                    std::fill(std::begin(m_loadScaleFactors), std::begin(m_loadScaleFactors) + 255, 1.0f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Zero")) {
                    std::fill(std::begin(m_loadScaleFactors), std::begin(m_loadScaleFactors) + 255, 0.0f);
                }
                m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::begin(m_loadScaleFactors) + 255);
            }
        }
        else
            ImGui::Text("No loads defined.");
    }
}
