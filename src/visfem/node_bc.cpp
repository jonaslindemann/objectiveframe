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
{}

void NodeBC::setNodeBC(ofem::BeamNodeBC *bc)
{
    m_bc.clear();
    m_reaction.clear();

    m_nodeBC = bc;

    double x, y, z;

    if (m_nodeBC != nullptr)
    {
        for (auto i = 0; i < m_nodeBC->getNodeSize(); i++)
        {
            auto ivfBC = vfem::BC::create();
            auto ivfReaction = vfem::Reaction::create();

            if (m_beamModel != nullptr)
            {
                ivfReaction->setBeamModel(m_beamModel);
                ivfBC->setBeamModel(m_beamModel);
            }

            auto node = m_nodeBC->getNode(i);

            m_nodeBC->getNode(i)->getCoord(x, y, z);
            ivfBC->setPosition(x, y, z);
            ivfBC->setMaterial(m_bcMaterial);
            m_bc.push_back(ivfBC);

            ivfReaction->setPosition(x, y, z);
            ivfReaction->setMaterial(m_bcMaterial);
            m_reaction.push_back(ivfReaction);

            if (m_beamModel != nullptr)
                m_beamModel->getColorTable()->assignColor(m_nodeBC->getColor(), m_bcMaterial);
        }
    }

    refresh();
}

void NodeBC::doCreateGeometry()
{
    for (auto &bc : m_bc)
        bc->render();

    for (auto &reaction : m_reaction)
        reaction->render();
}

void NodeBC::refresh()
{
    if (m_nodeBC != nullptr)
    {
        for (auto i = 0; i < m_nodeBC->getNodeSize(); i++)
        {
            double x, y, z;
            BCPtr ivfBC = m_bc[i];
            ReactionPtr ivfReaction = m_reaction[i];

            if (m_beamModel != nullptr)
            {
                ivfReaction->setBeamModel(m_beamModel);
                ivfBC->setBeamModel(m_beamModel);
            }

            auto node = m_nodeBC->getNode(i);

            for (auto j = 0; j < 6; j++)
            {
                if (m_nodeBC->isPrescribed(j + 1))
                    ivfBC->prescribe(j + 1, true);
                else
                    ivfBC->prescribe(j + 1, false);

                if (m_nodeBC->isPrescribed(j + 1))
                {
                    ivfReaction->show(j + 1, true);

                    if (node->getKind() == ofem::nk6Dof)
                    {
                        auto fx = node->getValue(6);
                        auto fy = node->getValue(7);
                        auto fz = node->getValue(8);
                        auto mx = node->getValue(9);
                        auto my = node->getValue(10);
                        auto mz = node->getValue(11);
                        ivfReaction->setMaxForce(m_beamModel->getMaxReactionForce());
                        ivfReaction->setMaxMoment(m_beamModel->getMaxReactionMoment());
                        ivfReaction->setForces(fx, fy, fz);
                        ivfReaction->setMoments(mx, my, mz);
                    }
                    else if (node->getKind() == ofem::nk3Dof)
                    {
                        auto fx = node->getValue(3);
                        auto fy = node->getValue(4);
                        auto fz = node->getValue(5);
                        ivfReaction->setMaxForce(m_beamModel->getMaxReactionForce());
                        ivfReaction->setMaxMoment(m_beamModel->getMaxReactionMoment());
                        ivfReaction->setForces(fx, fy, fz);
                        ivfReaction->setMoments(0.0, 0.0, 0.0);
                    }
                    else
                        ivfReaction->show(j + 1, false);
                }
                else
                    ivfReaction->show(j + 1, false);
            }
            m_nodeBC->getNode(i)->getCoord(x, y, z);
            ivfBC->setPosition(x, y, z);
            ivfBC->setMaterial(m_bcMaterial);

            ivfReaction->setPosition(x, y, z);
            ivfReaction->setMaterial(m_bcMaterial);

            if (m_beamModel != nullptr)
                m_beamModel->getColorTable()->assignColor(m_nodeBC->getColor(), m_bcMaterial);
        }
    }
}

void NodeBC::setBeamModel(BeamModel *model)
{
    m_beamModel = model;
}
