#include <ofui/materials_window.h>

#include <imgui.h>

#include <FemWidget.h>

using namespace ofem;
using namespace ofui;

MaterialsWindow::MaterialsWindow(const std::string name)
    : UiWindow(name)
    , m_materials { nullptr }
    , m_currentItemIdx { -1 }
    , m_widget { nullptr }
{
    m_propPopup = MaterialPropPopup::create("Material", true);
    m_propPopup->setVisible(false);
}

MaterialsWindow::~MaterialsWindow()
{
}

std::shared_ptr<MaterialsWindow> MaterialsWindow::create(const std::string name)
{
    return std::make_shared<MaterialsWindow>(name);
}

void MaterialsWindow::setFemMaterialSet(BeamMaterialSet* materialSet)
{
    m_materials = materialSet;
    m_selected.resize(m_materials->getSize(), false);
}

void MaterialsWindow::setFemWidget(FemWidget* widget)
{
    m_widget = widget;
    m_propPopup->setFemWidget(widget);
}

void MaterialsWindow::doPreDraw()
{
}

void MaterialsWindow::doDraw()
{
    ImGui::BeginGroup();

    if (ImGui::BeginListBox("##empty", ImVec2(0.0f, -FLT_MIN)))
    {
        if (m_materials != nullptr)
        {
            for (auto i = 0; i < m_materials->getSize(); i++)
            {
                BeamMaterial* material = static_cast<BeamMaterial*>(m_materials->getMaterial(i));

                ImGui::PushID(i);
                if (ImGui::Selectable(material->getName().c_str(), i == m_currentItemIdx))
                {
                    m_currentItemIdx = i;
                    m_widget->setCurrentMaterial(material);
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
        if (m_materials != nullptr)
        {
            auto material = new BeamMaterial();
            material->setName("new material");
            m_materials->addMaterial(material);
            // m_widget->addBeamLoad(load);
        }
    }
    if (ImGui::Button("Remove", ImVec2(100.0f, 0.0f)))
    {
        if (m_materials != nullptr)
        {
            if (m_currentItemIdx != -1)
            {
                auto material = m_materials->getMaterial(m_currentItemIdx);
                m_materials->removeMaterial(material);
            }
        }
    }
    if (ImGui::Button("Assign", ImVec2(100.0f, 0.0f)))
    {
        if (m_materials != nullptr)
        {
            m_widget->assignMaterialToSelected();
            m_widget->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Unassign", ImVec2(100.0f, 0.0f)))
    {
        if (m_materials != nullptr)
        {
            m_widget->removeMaterialFromSelected();
            m_widget->setNeedRecalc(true);
        }
    }
    if (ImGui::Button("Properties...", ImVec2(100.0f, 0.0f)))
    {
        if (m_materials != nullptr)
        {
            if (m_widget->getCurrentMaterial() != nullptr)
            {
                m_propPopup->setVisible(true);
            }
        }
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

void MaterialsWindow::doPostDraw()
{
}
