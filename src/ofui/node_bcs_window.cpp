#include <ofui/node_bcs_window.h>

#include <imgui.h>

#include <FemWidget.h>

using namespace ofem;
using namespace ofui;

NodeBCsWindow::NodeBCsWindow(const std::string name)
    : UiWindow(name)
    , m_femNodeBCSet { nullptr }
    , m_currentItemIdx { -1 }
    , m_widget { nullptr }
{
    m_propPopup = BCPropPopup::create("Node BC", true);
    m_propPopup->setVisible(false);
}

NodeBCsWindow::~NodeBCsWindow()
{
}

std::shared_ptr<NodeBCsWindow> NodeBCsWindow::create(const std::string name)
{
    return std::make_shared<NodeBCsWindow>(name);
}

void NodeBCsWindow::setFemNodeBCSet(BeamNodeBCSet* bcSet)
{
    m_femNodeBCSet = bcSet;
    m_selected.resize(m_femNodeBCSet->getSize(), false);
}

void NodeBCsWindow::setFemWidget(FemWidget* widget)
{
    m_widget = widget;
    m_propPopup->setFemWidget(widget);
}

void NodeBCsWindow::doPreDraw()
{
}

void NodeBCsWindow::doDraw()
{
    ImGui::BeginGroup();

    if (ImGui::BeginListBox("##empty", ImVec2(0.0f, -FLT_MIN)))
    {
        if (m_femNodeBCSet != nullptr)
        {
            for (size_t i = 0; i < m_femNodeBCSet->getSize(); i++)
            {
                BeamNodeBC* nodeBC = static_cast<BeamNodeBC*>(m_femNodeBCSet->getBC(static_cast<long>(i)));

                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(nodeBC->getName().c_str(), i == m_currentItemIdx))
                {
                    m_currentItemIdx = static_cast<int>(i);
                    m_widget->setCurrentNodeBC(nodeBC);
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
        if (m_femNodeBCSet != nullptr)
        {
            BeamNodeBC* bc = new BeamNodeBC();
            bc->setName("new bc");
            m_femNodeBCSet->addBC(bc);
        }
    }
    if (ImGui::Button("Remove", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeBCSet != nullptr)
        {
            if (m_currentItemIdx != -1)
            {
                m_widget->removeBCsFromBC();
                m_femNodeBCSet->removeBC(m_currentItemIdx);
            }
        }
    }
    if (ImGui::Button("Assign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeBCSet != nullptr)
        {
            m_widget->assignNodeBCSelected();
            m_widget->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Unassign", ImVec2(100.0f, 0.0f)))
    {
        if (m_femNodeBCSet != nullptr)
        {
            m_widget->removeNodeBCsFromSelected();
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
        }
    }
}

void NodeBCsWindow::doPostDraw()
{
}
