#include <ofui/prop_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

const double PI = 2 * acos(0.0);

PropWindow::PropWindow(const std::string name)
    : UiWindow(name), m_beam{nullptr}, m_beamRotation{0.0f}, m_oldBeamRotation{0.0f}, m_view{nullptr}, m_beamType{0},
      m_selectedShapes{nullptr}, m_node{nullptr}, m_nodePos{0.0, 0.0, 0.0}, m_nodeDispl{0.0, 0.0, 0.0}, m_nodeMove{0.0,
                                                                                                                   0.0,
                                                                                                                   0.0}
{
}

PropWindow::~PropWindow()
{
}

void PropWindow::setBeam(vfem::Beam *beam)
{
    m_beam = beam;
    if (m_beam != nullptr) {
        m_beamRotation = float(m_beam->getBeam()->getBeamRotation() * 2.0f * PI / 360.0f);
        m_oldBeamRotation = m_beamRotation;

        if (m_beam->getBeam()->beamType() == ofem::btBeam)
            m_beamType = 0;
        else
            m_beamType = 1;
    }
}

void PropWindow::setNode(vfem::Node *node)
{
    m_node = node;
}

void ofui::PropWindow::setSelectedShapes(ivf::Composite *selected)
{
    m_selectedShapes = selected;
}

#ifdef USE_FEMVIEW
void ofui::PropWindow::setView(FemViewWindow *view)
{
    m_view = view;
}
#else
void PropWindow::setWidget(FemWidget *widget)
{
    m_view = widget;
}
#endif

std::shared_ptr<PropWindow> PropWindow::create(const std::string name)
{
    return std::make_shared<PropWindow>(name);
}

void PropWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    int nodeCount = 0;
    int beamCount = 0;

    if (m_node != nullptr) {
        ImGui::Text("Node properties");
        ImGui::Separator();

        double x, y, z;
        m_node->getPosition(x, y, z);
        m_nodePos[0] = float(x);
        m_nodePos[1] = float(y);
        m_nodePos[2] = float(z);
        ImGui::InputFloat3("Position", m_nodePos, "%.4g");
        m_node->setPosition(m_nodePos[0], m_nodePos[1], m_nodePos[2]);

        double dx, dy, dz;
        dx = m_node->getFemNode()->getValue(0);
        dy = m_node->getFemNode()->getValue(1);
        dz = m_node->getFemNode()->getValue(2);

        m_nodeDispl[0] = float(dx);
        m_nodeDispl[1] = float(dy);
        m_nodeDispl[2] = float(dz);
        ImGui::InputFloat3("Displacement", m_nodeDispl, "%.4g", ImGuiInputTextFlags_ReadOnly);
    }
    else if (m_selectedShapes != nullptr) {
        nodeCount = m_view->selectionShapeCount("vfem::Node");

        if (nodeCount > 0) {
            ImGui::Text("Node properties");
            ImGui::Separator();

            ImGui::InputFloat3("Offset", m_nodeMove, "%.4g");

            ImGui::Separator();

            if (ImGui::Button("Move", ImVec2(120, 0))) {
                m_view->snapShot();

                for (auto i = 0; i < m_selectedShapes->getSize(); i++) {
                    if (m_selectedShapes->getChild(i)->isClass("vfem::Node")) {
                        auto node = static_cast<vfem::Node *>(m_selectedShapes->getChild(i));
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

            if (ImGui::Button("Copy", ImVec2(120, 0))) {
                std::vector<ivf::Shape *> newSelected;

                m_view->snapShot();

                for (auto i = 0; i < m_selectedShapes->getSize(); i++) {
                    if (m_selectedShapes->getChild(i)->isClass("vfem::Node")) {
                        auto node = static_cast<vfem::Node *>(m_selectedShapes->getChild(i));
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

            if (ImGui::Button("Reset", ImVec2(120, 0))) {
                m_nodeMove[0] = 0.0;
                m_nodeMove[1] = 0.0;
                m_nodeMove[2] = 0.0;
            }

            ImGui::Separator();
        }
    }

    if (m_beam != nullptr) {
        ImGui::Text("Beam properties");
        ImGui::Separator();

        ImGui::SliderAngle("Rotation", &m_beamRotation, -180.0f, 180.0f);

        if (m_beam->getBeam()->beamType() == ofem::btBeam)
            m_beamType = 0;
        else
            m_beamType = 1;

        ImGui::RadioButton("Beam", &m_beamType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Bar", &m_beamType, 1);

        if (m_beamType == 0) {
            m_beam->getBeam()->setBeamType(ofem::btBeam);
            m_beam->refresh();
        }
        else {
            m_beam->getBeam()->setBeamType(ofem::btBar);
            m_beam->refresh();
        }

        if (m_beam->getBeam()->getValueSize() > 0) {
            if (ImGui::BeginTabBar("Test")) {
                if (ImGui::BeginTabItem("Section forces")) {
                    int n = m_beam->getBeam()->getEvaluationPoints();
                    size_t k = m_beam->getBeam()->getValueSize();

                    int pos = 0;

                    // N          Vy         Vz         T          My         Mz

                    ImGui::BeginTable("SectForces", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("N");
                    ImGui::TableNextColumn();
                    ImGui::Text("Vy");
                    ImGui::TableNextColumn();
                    ImGui::Text("Vz");
                    ImGui::TableNextColumn();
                    ImGui::Text("T");
                    ImGui::TableNextColumn();
                    ImGui::Text("My");
                    ImGui::TableNextColumn();
                    ImGui::Text("Mz");

                    for (auto i = 0; i < n; i++) {
                        ImGui::TableNextRow();
                        for (auto j = 0; j < 6; j++) {
                            ImGui::TableNextColumn();
                            ImGui::Text("%.4g", m_beam->getBeam()->getValue(pos++));
                        }
                    }
                    ImGui::EndTable();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Displacements")) {
                    int n = m_beam->getBeam()->getEvaluationPoints();
                    size_t k = m_beam->getBeam()->getValueSize();

                    int pos = 6 * n;

                    ImGui::BeginTable("Displ", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("u");
                    ImGui::TableNextColumn();
                    ImGui::Text("w");
                    ImGui::TableNextColumn();
                    ImGui::Text("v");
                    ImGui::TableNextColumn();
                    ImGui::Text("phi");

                    for (auto i = 0; i < n; i++) {
                        ImGui::TableNextRow();
                        for (auto j = 0; j < 4; j++) {
                            ImGui::TableNextColumn();
                            ImGui::Text("%.4g", m_beam->getBeam()->getValue(pos++));
                        }
                    }
                    ImGui::EndTable();
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
    else {
        if (m_selectedShapes != nullptr) {
            beamCount = m_view->selectionShapeCount("vfem::Beam");

            if (beamCount > 0) {
                ImGui::Text("Beam properties");
                ImGui::Separator();

                ImGui::SliderAngle("Rotation", &m_beamRotation, -180.0f, 180.0f);

                ImGui::RadioButton("Beam", &m_beamType, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Bar", &m_beamType, 1);

                for (auto i = 0; i < m_selectedShapes->getSize(); i++) {
                    auto shape = m_selectedShapes->getChild(i);
                    if (shape->isClass("vfem::Beam")) {
                        auto beam = static_cast<vfem::Beam *>(shape);
                        if (m_beamType == 0)
                            beam->getBeam()->setBeamType(ofem::btBeam);
                        else
                            beam->getBeam()->setBeamType(ofem::btBar);

                        beam->refresh();

                        m_view->setNeedRecalc(true);
                    }
                }
            }
        }
    }

    if ((m_beam == nullptr) && (m_node == nullptr) && (beamCount == 0) && (nodeCount == 0)) {
        ImGui::Text("Select an object to display properties.");
    }

    if (m_oldBeamRotation != m_beamRotation) {
        if (m_view != nullptr) {
            m_view->setRotationSelected(double(m_beamRotation * 360.0f / 2.0f / PI));
        }
    }
}
