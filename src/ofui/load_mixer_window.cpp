#include <ofui/load_mixer_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

#include <algorithm>
#include <cmath>
#include <numeric>

using namespace ofui;

LoadMixerWindow::LoadMixerWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_femNodeLoadSet{nullptr}, m_loadSum{0.0}, m_lastSum{0.0}, m_firstUpdate{true}
{
    std::fill(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors), 1.0f);
    m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors));
    m_lastSum = m_loadSum;
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
    std::fill(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors), 1.0f);

    if (m_femNodeLoadSet != nullptr)
    {
        int count = std::min(static_cast<int>(m_femNodeLoadSet->getSize()), MaxLoadScaleFactors);
        for (int i = 0; i < count; i++)
        {
            auto nodeLoad = static_cast<ofem::BeamNodeLoad *>(m_femNodeLoadSet->getLoad(i));
            m_loadScaleFactors[i] = static_cast<float>(nodeLoad->getScale());
        }
    }

    m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors));
    m_lastSum = m_loadSum;
    m_firstUpdate = true;
}

void LoadMixerWindow::update()
{
    if (m_view != nullptr) {
        if (m_femNodeLoadSet != nullptr) {
            bool scaleChanged = false;
            int count = std::min(static_cast<int>(m_femNodeLoadSet->getSize()), MaxLoadScaleFactors);
            for (auto i = 0; i < count; i++) {
                auto nodeLoad = static_cast<ofem::BeamNodeLoad *>(m_femNodeLoadSet->getLoad(i));
                if (std::fabs(nodeLoad->getScale() - m_loadScaleFactors[i]) > 1.0e-6f)
                    scaleChanged = true;
                nodeLoad->setScale(m_loadScaleFactors[i]);
            }

            if (scaleChanged && !m_firstUpdate) {
                m_view->recompute();
            }

            m_firstUpdate = false;

            m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors));
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
                int count = std::min(static_cast<int>(m_femNodeLoadSet->getSize()), MaxLoadScaleFactors);
                for (auto i = 0; i < count; i++) {
                    auto nodeLoad = static_cast<ofem::BeamNodeLoad *>(m_femNodeLoadSet->getLoad(i));

                    ImGui::SliderFloat(nodeLoad->getName().c_str(), &m_loadScaleFactors[i], -10.0, 10.0);
                }

                ImGui::Separator();

                if (ImGui::Button("Reset")) {
                    std::fill(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors), 1.0f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Zero")) {
                    std::fill(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors), 0.0f);
                }
                m_loadSum = std::reduce(std::begin(m_loadScaleFactors), std::end(m_loadScaleFactors));
            }
        }
        else
            ImGui::Text("No loads defined.");
    }
}
