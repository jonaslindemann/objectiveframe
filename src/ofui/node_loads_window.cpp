#include <ofui/node_loads_window.h>

#include <imgui.h>

#include <FemView.h>

using namespace ofem;
using namespace ofui;

NodeLoadsWindow::NodeLoadsWindow(const std::string name)
    : UiWindow(name)
    , m_femNodeLoadSet { nullptr }
    , m_currentItemIdx { -1 }
    , m_view { nullptr }
{
    m_propPopup = NodeLoadPropPopup::create("Node Load", true);
    m_propPopup->setVisible(false);
}

NodeLoadsWindow::~NodeLoadsWindow()
{
}

std::shared_ptr<NodeLoadsWindow> NodeLoadsWindow::create(const std::string name)
{
    return std::make_shared<NodeLoadsWindow>(name);
}

void NodeLoadsWindow::setFemNodeLoadSet(BeamNodeLoadSet* bcSet)
{
    m_femNodeLoadSet = bcSet;
    m_selected.resize(m_femNodeLoadSet->getSize(), false);
}

#ifdef USE_FEMVIEW
void ofui::NodeLoadsWindow::setFemView(FemViewWindow* view)
{
    m_view = view;
    m_propPopup->setFemView(view);
}
#else
void NodeLoadsWindow::setFemWidget(FemWidget* widget)
{
    m_view = widget;
    m_propPopup->setFemWidget(widget);
}
#endif

void NodeLoadsWindow::doPreDraw()
{
}

void NodeLoadsWindow::doDraw()
{
    ImGui::BeginGroup();

    if (ImGui::BeginListBox("##empty", ImVec2(0.0f, -FLT_MIN)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            for (auto i = 0; i < m_femNodeLoadSet->getSize(); i++)
            {
                BeamNodeLoad* nodeLoad = static_cast<BeamNodeLoad*>(m_femNodeLoadSet->getLoad(i));

                ImGui::PushID(i);
                if (ImGui::Selectable(nodeLoad->getName().c_str(), i == m_currentItemIdx))
                {
                    m_currentItemIdx = i;
                    m_view->setCurrentNodeLoad(nodeLoad);
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
    if (ImGui::Button("Add", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            BeamNodeLoad* load = new BeamNodeLoad();
            load->setName("new load");
            m_femNodeLoadSet->addLoad(load);
            m_view->addNodeLoad(load);
        }
    }
    if (ImGui::Button("Remove", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            if (m_currentItemIdx != -1)
            {
                auto nodeLoad = static_cast<ofem::BeamNodeLoad*>(m_femNodeLoadSet->getLoad(m_currentItemIdx));
                m_view->removeNodesFromNodeLoad();
                m_view->deleteNodeLoad(nodeLoad);
                m_view->setNeedRecalc(true);
            }
        }
    }
    if (ImGui::Button("Assign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            m_view->assignNodeLoadSelected();
            m_view->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Unassign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            m_view->removeNodeLoadsFromSelected();
            m_view->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Properties...", ImVec2(100.0f, 0.0f)))
    {
        m_propPopup->setVisible(true);
    }

    ImGui::EndGroup();

    m_propPopup->draw();

    if (m_propPopup->closed())
    {
        if (m_propPopup->modalResult() == PopupResult::OK)
        {
        }
        else if (m_propPopup->modalResult() == PopupResult::CANCEL)
        {
        }
    }
}

void NodeLoadsWindow::doPostDraw()
{
}
