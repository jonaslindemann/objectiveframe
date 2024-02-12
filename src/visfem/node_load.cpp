#include <vfem/node_load.h>

#include <cmath>

using namespace std;
using namespace ivf;
using namespace vfem;

NodeLoad::NodeLoad() : Shape()
{
    m_beamModel = nullptr;
    m_nodeLoad = nullptr;

    m_arrowMaterial = ivf::Material::create();
    m_arrowMaterial->setAmbientColor(0.3f, 0.0f, 0.0f, 1.0f);
    m_arrowMaterial->setDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
    m_arrowMaterial->setSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
    this->setMaterial(m_arrowMaterial);
    this->setUseName(true);
    this->setUseSelectShape(false);
}

NodeLoad::~NodeLoad()
{}

void NodeLoad::doCreateGeometry()
{
    for (auto &arrow : m_arrow)
        arrow->render();
}

void NodeLoad::doCreateSelect()
{}

void NodeLoad::initArrow()
{
    double x, y, z, ex, ey, ez;
    double loadHeight;
    double l;

    if (m_nodeLoad != nullptr)
    {

        for (int i = 0; i < m_arrow.size(); i++)
        {
            if (m_beamModel != nullptr)
                loadHeight = m_beamModel->getLoadSize();
            else
                loadHeight = 1.0;

            // Set topolgy

            auto node = m_nodeLoad->getNode(i);

            // Define arrow

            m_arrow[i]->setSize(loadHeight * 0.6, loadHeight * 0.6 * 0.20);
            m_arrow[i]->setRadius(loadHeight * 0.05, loadHeight * 0.03);

            // Calculate arrow position

            node->getCoord(x, y, z);

            // Calculate arrow direction

            m_nodeLoad->getDirection(ex, ey, ez);

            l = sqrt(pow(ex, 2) + pow(ey, 2) + pow(ez, 2));
            ex = ex / l;
            ey = ey / l;
            ez = ez / l;

            double sign = 1.0;
            if (m_nodeLoad->getValue() < 0)
                sign = -1.0;

            m_arrow[i]->setDirection(sign * ex, sign * ey, sign * ez);
            m_arrow[i]->setPosition(x - sign * 0.7 * loadHeight * ex, y - sign * 0.7 * loadHeight * ey,
                                    z - sign * 0.7 * loadHeight * ez);
            m_arrow[i]->refresh();
            if (m_beamModel != nullptr)
                m_beamModel->getColorTable()->assignColor(m_nodeLoad->getColor(), m_arrowMaterial);
        }
    }
}

void NodeLoad::refresh()
{
    setNodeLoad(m_nodeLoad);
    initArrow();
}

void NodeLoad::setNodeLoad(ofem::BeamNodeLoad *nodeLoad)
{
    m_arrow.clear();

    m_nodeLoad = nodeLoad;

    if (m_nodeLoad != nullptr)
    {
        for (unsigned int i = 0; i < m_nodeLoad->getNodeSize(); i++)
        {
            auto arrow = ExtrArrow::create();
            m_arrow.push_back(arrow);
            arrow->setMaterial(m_arrowMaterial);
        }
    }
}

void NodeLoad::setBeamModel(BeamModel *model)
{
    m_beamModel = model;
}
