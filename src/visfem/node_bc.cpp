#include <vfem/node_bc.h>

#include <ivf/Cone.h>
#include <ivf/Transform.h>

using namespace ivf;
using namespace vfem;


NodeBC::NodeBC()
{
    this->setUseSelectShape(false);

    m_bcMaterial = ivf::Material::create();
    m_bcMaterial->setDiffuseColor(1.0, 1.0, 1.0, 1.0);

    m_nodeBC = nullptr;

    m_beamModel = nullptr;
}


NodeBC::~NodeBC()
{
}


void NodeBC::setNodeBC(ofem::BeamNodeBC* bc)
{
    int i, j;

    for (i = 0; i < m_bc.size(); i++)
    {
        delete m_bc[i];
    }
    m_bc.clear();

    m_nodeBC = bc;
    if (m_nodeBC != nullptr)
    {
        for (i = 0; i < m_nodeBC->getNodeSize(); i++)
        {
            double x, y, z;
            vfem::BC* ivfBC = new vfem::BC();
            if (m_beamModel != nullptr)
                ivfBC->setBeamModel(m_beamModel);
            for (j = 0; j < 6; j++)
            {
                if (m_nodeBC->isPrescribed(j + 1))
                    ivfBC->prescribe(j + 1, true);
                else
                    ivfBC->prescribe(j + 1, false);
            }
            m_bc.push_back(ivfBC);
            ivfBC->setMaterial(m_bcMaterial);
            m_nodeBC->getNode(i)->getCoord(x, y, z);
            ivfBC->setPosition(x, y, z);
            if (m_beamModel != nullptr)
                m_beamModel->getColorTable()->assignColor(m_nodeBC->getColor(), m_bcMaterial);
        }
    }
}


void NodeBC::doCreateGeometry()
{
    int i;

    for (i = 0; i < m_bc.size(); i++)
        m_bc[i]->render();
}


void NodeBC::refresh()
{
    setNodeBC(m_nodeBC);
}

void NodeBC::setBeamModel(BeamModel* model)
{
    m_beamModel = model;
}
