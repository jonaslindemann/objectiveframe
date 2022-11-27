#include <ofui/node_prop_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

NodePropWindow::NodePropWindow(const std::string name)
    : UiWindow(name)
    , m_node { nullptr }
    , m_nodePos { 0.0, 0.0, 0.0 }
    , m_nodeDispl { 0.0, 0.0, 0.0 }
    , m_nodeMove { 0.0, 0.0, 0.0 }
    , m_selectedShapes { nullptr }
    , m_view { nullptr }
{
}

NodePropWindow::~NodePropWindow()
{
}

#ifdef USE_FEMVIEW
void ofui::NodePropWindow::setView(FemViewWindow* view)
{
    m_view = view;
}
#else
void ofui::NodePropWindow::setWidget(FemWidget* widget)
{
    m_view = widget;
}
#endif

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
    ImGuiIO& io = ImGui::GetIO();
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
            m_view->snapShot();

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
            m_view->getScene()->getComposite()->refresh();
            m_view->set_changed();
            m_view->redraw();
        }

        ImGui::SameLine();

        if (ImGui::Button("Copy", ImVec2(120, 0)))
        {
            std::vector<ivf::Shape*> newSelected;

            m_view->snapShot();

            for (auto i = 0; i < m_selectedShapes->getSize(); i++)
            {
                if (m_selectedShapes->getChild(i)->isClass("vfem::Node"))
                {
                    auto node = static_cast<vfem::Node*>(m_selectedShapes->getChild(i));
                    double x, y, z;
                    node->getPosition(x, y, z);
                    auto newNode = m_view->addNode(x + m_nodeMove[0], y + m_nodeMove[1], z + m_nodeMove[2]);
                    newSelected.push_back(newNode);
                }
            }
            m_view->clearSelection();

            for (auto node : newSelected)
                m_view->addSelection(node);

            m_view->getScene()->getComposite()->refresh();
            m_view->set_changed();
            m_view->redraw();
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset", ImVec2(120, 0)))
        {
            m_nodeMove[0] = 0.0;
            m_nodeMove[1] = 0.0;
            m_nodeMove[2] = 0.0;
        }

        ImGui::Separator();

        /*
        ImGui::TextUnformatted("Shortcuts");

        if (ImGui::Button("Up(Y)"))
        {
        }

        ImGui::SameLine();

        if (ImGui::Button("Down(Z)"))
        {
        }

        ImGui::SameLine();

        if (ImGui::Button("Left(X)"))
        {
        }

        ImGui::SameLine();

        if (ImGui::Button("Right(X)"))
        {
        }

        ImGui::SameLine();

        if (ImGui::Button("Forward(Z)"))
        {
        }

        ImGui::SameLine();

        if (ImGui::Button("Back(Z)"))
        {
        }

        */
    }
    else
    {
        ImGui::Text("Select a node(s) to display node properties.");
    }
}
