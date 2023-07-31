#include <ofui/element_loads_window.h>

#include <imgui.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofem;
using namespace ofui;

ElementLoadsWindow::ElementLoadsWindow(const std::string name)
    : UiWindow(name), m_femBeamLoadSet{nullptr}, m_currentItemIdx{-1}, m_view{nullptr}
{
    m_propPopup = ElementLoadPropPopup::create("Element Load", true);
    m_propPopup->setVisible(false);
}

ElementLoadsWindow::~ElementLoadsWindow()
{
}

std::shared_ptr<ElementLoadsWindow> ElementLoadsWindow::create(const std::string name)
{
    return std::make_shared<ElementLoadsWindow>(name);
}

void ElementLoadsWindow::setFemLoadSet(BeamLoadSet *bcSet)
{
    m_femBeamLoadSet = bcSet;
    m_selected.resize(m_femBeamLoadSet->getSize(), false);
}

#ifdef USE_FEMVIEW
void ofui::ElementLoadsWindow::setFemView(FemViewWindow *view)
{
    m_view = view;
}
#else
void ElementLoadsWindow::setFemWidget(FemWidget *widget)
{
    m_view = widget;
    m_propPopup->setFemWidget(widget);
}
#endif

void ElementLoadsWindow::doPreDraw()
{
}

void ElementLoadsWindow::doDraw()
{
    ImGui::BeginGroup();

    if (ImGui::BeginListBox("##empty", ImVec2(0.0f, -FLT_MIN))) {
        if (m_femBeamLoadSet != nullptr) {
            for (auto i = 0; i < m_femBeamLoadSet->getSize(); i++) {
                ofem::BeamLoad *beamLoad = static_cast<ofem::BeamLoad *>(m_femBeamLoadSet->getLoad(i));

                ImGui::PushID(i);
                if (ImGui::Selectable(beamLoad->getName().c_str(), i == m_currentItemIdx)) {
                    m_currentItemIdx = i;
                    m_view->setCurrentBeamLoad(beamLoad);
                    m_propPopup->update();
                }
                ImGui::PopID();
            }
        }
        ImGui::EndListBox();
    }
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    if (ImGui::Button("Add", ImVec2(100.0f, 0.0f))) {
        if (m_femBeamLoadSet != nullptr) {
            ofem::BeamLoad *load = new ofem::BeamLoad();
            load->setName("new load");
            m_femBeamLoadSet->addLoad(load);
            m_view->addBeamLoad(load);
        }
    }
    if (ImGui::Button("Remove", ImVec2(100.0f, 0.0f))) {
        if (m_femBeamLoadSet != nullptr) {
            if (m_currentItemIdx != -1) {
                m_view->removeNodesFromNodeLoad();
                m_femBeamLoadSet->removeLoad(m_currentItemIdx);
            }
        }
    }
    if (ImGui::Button("Assign", ImVec2(100.0f, 0.0f))) {
        if (m_femBeamLoadSet != nullptr) {
            m_view->assignBeamLoadSelected();
            m_view->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Unassign", ImVec2(100.0f, 0.0f))) {
        if (m_femBeamLoadSet != nullptr) {
            m_view->removeBeamLoadsFromSelected();
            m_view->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Properties...", ImVec2(100.0f, 0.0f))) {
        m_propPopup->setVisible(true);
    }

    ImGui::EndGroup();

    m_propPopup->draw();

    if (m_propPopup->closed()) {
        if (m_propPopup->modalResult() == PopupResult::OK) {
        }
        else if (m_propPopup->modalResult() == PopupResult::CANCEL) {
        }
    }
}

void ElementLoadsWindow::doPostDraw()
{
}
