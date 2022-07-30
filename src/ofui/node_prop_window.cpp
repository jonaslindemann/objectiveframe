#include <ofui/node_prop_window.h>

#include <FemWidget.h>

using namespace ofui;

NodePropWindow::NodePropWindow(const std::string name)
    : UiWindow(name)
    , m_node { nullptr }
    , m_nodePos { 0.0, 0.0, 0.0 }
    , m_nodeDispl { 0.0, 0.0, 0.0 }
    , m_nodeMove { 0.0, 0.0, 0.0 }
    , m_selectedShapes { nullptr }
    , m_widget { nullptr }
{
}

NodePropWindow::~NodePropWindow()
{
}

void ofui::NodePropWindow::setWidget(FemWidget* widget)
{
    m_widget = widget;
}

void NodePropWindow::setNode(vfem::Node* node)
{
    m_node = node;
}

void NodePropWindow::setSelectedShapes(ivf::Composite* selected)
{
    m_selectedShapes = selected;
}

std::shared_ptr<NodePropWindow> NodePropWindow::create(const std::string name)
{
    return std::make_shared<NodePropWindow>(name);
}

void NodePropWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));
    if (m_node != nullptr)
    {
        double x, y, z;
        m_node->getPosition(x, y, z);
        m_nodePos[0] = float(x);
        m_nodePos[1] = float(y);
        m_nodePos[2] = float(z);
        ImGui::InputFloat3("Position", m_nodePos, "%.3f");
        m_node->setPosition(m_nodePos[0], m_nodePos[1], m_nodePos[2]);

        double dx, dy, dz;
        dx = m_node->getFemNode()->getValue(0);
        dy = m_node->getFemNode()->getValue(1);
        dz = m_node->getFemNode()->getValue(2);

        m_nodeDispl[0] = float(dx);
        m_nodeDispl[1] = float(dy);
        m_nodeDispl[2] = float(dz);
        ImGui::InputFloat3("Displacement", m_nodeDispl, "%.3f", ImGuiInputTextFlags_ReadOnly);
    }
    else if (m_selectedShapes != nullptr)
    {
        ImGui::InputFloat3("Offset", m_nodeMove, "%.3f");
        ImGui::Separator();

        if (ImGui::Button("Move", ImVec2(120, 0)))
        {
            for (auto i = 0; i < m_selectedShapes->getSize(); i++)
            {
                if (m_selectedShapes->getChild(i)->isClass("vfem::Node"))
                {
                    auto node = static_cast<vfem::Node*>(m_selectedShapes->getChild(i));
                    double x, y, z;
                    node->getPosition(x, y, z);
                    node->setPosition(x + m_nodeMove[0], y + m_nodeMove[1], z + m_nodeMove[2]);
                }
            }
            m_widget->getScene()->getComposite()->refresh();
            m_widget->set_changed();
            m_widget->redraw();
        }

        ImGui::SameLine();

        if (ImGui::Button("Copy", ImVec2(120, 0)))
        {
            std::vector<ivf::Shape*> newSelected;
            for (auto i = 0; i < m_selectedShapes->getSize(); i++)
            {
                if (m_selectedShapes->getChild(i)->isClass("vfem::Node"))
                {
                    auto node = static_cast<vfem::Node*>(m_selectedShapes->getChild(i));
                    double x, y, z;
                    node->getPosition(x, y, z);
                    auto newNode = m_widget->addNode(x + m_nodeMove[0], y + m_nodeMove[1], z + m_nodeMove[2]);
                    newSelected.push_back(newNode);
                }
            }
            m_widget->clearSelection();

            for (auto node : newSelected)
                m_widget->addSelection(node);

            m_widget->getScene()->getComposite()->refresh();
            m_widget->set_changed();
            m_widget->redraw();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", ImVec2(120, 0)))
        {
            m_nodeMove[0] = 0.0;
            m_nodeMove[1] = 0.0;
            m_nodeMove[2] = 0.0;
        }
    }
    else
    {
        ImGui::Text("Select a node(s) to display node properties.");
    }
}
