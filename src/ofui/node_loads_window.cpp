#include <ofui/node_loads_window.h>

#include <imgui.h>

#include <FemWidget.h>

using namespace ofem;
using namespace ofui;

NodeLoadsWindow::NodeLoadsWindow(const std::string name)
    : UiWindow(name)
    , m_femNodeLoadSet { nullptr }
    , m_currentItemIdx { -1 }
    , m_widget { nullptr }
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

void NodeLoadsWindow::setFemWidget(FemWidget* widget)
{
    m_widget = widget;
    m_propPopup->setFemWidget(widget);
}

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
                    std::cout << ">" << m_currentItemIdx << std::endl;
                    m_widget->setCurrentNodeLoad(nodeLoad);
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
            m_widget->addNodeLoad(load);
        }
    }
    if (ImGui::Button("Remove", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            if (m_currentItemIdx != -1)
            {
                m_widget->removeNodesFromNodeLoad();
                m_femNodeLoadSet->removeLoad(m_currentItemIdx);
            }
        }
    }
    if (ImGui::Button("Assign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            m_widget->assignNodeLoadSelected();
            m_widget->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Unassign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeLoadSet != nullptr)
        {
            m_widget->removeNodeLoadsFromSelected();
            m_widget->setNeedRecalc(true);
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
            cout << "Cancel pressed" << endl;
        }
    }
}

void NodeLoadsWindow::doPostDraw()
{
}
