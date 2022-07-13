// Implementation of: public class VisFemNode

#include "VisFemNode.h"

using namespace ivf;

// ------------------------------------------------------------
VisFemNode::VisFemNode ()
    :ivf::Node()
{
    m_femNode = NULL;
    m_directRefresh = false;

    m_beamModel = NULL;

    this->setUseSelectShape(false);
    this->setType(Node::NT_SPHERE);
}

// ------------------------------------------------------------
VisFemNode::~VisFemNode ()
{
}

// ------------------------------------------------------------
void VisFemNode::setFemNode(FemNode *node)
{
    double x, y, z;
    m_femNode = node;
    m_femNode->getCoord(x, y, z);
    Shape::setPosition(x, y, z);
}

// ------------------------------------------------------------
FemNode* VisFemNode::getFemNode()
{
    return m_femNode;
}

// ------------------------------------------------------------
void VisFemNode::setPosition (const double x, const double y, const double z)
{
    if (m_femNode!=NULL)
        m_femNode->setCoord(x, y, z);
    ivf::Shape::setPosition(x, y, z);
}

// ------------------------------------------------------------
void VisFemNode::setPosition (ivf::Shape* shape)
{
    if (m_femNode!=NULL)
    {
        double x, y, z;
        shape->getPosition(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    ivf::Shape::setPosition(shape);
}

// ------------------------------------------------------------
void VisFemNode::setPositionVec (ivf::Vec3d* point)
{
    if (m_femNode!=NULL)
    {
        double x, y, z;
        point->getComponents(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    ivf::Shape::setPosition(*point);
}

// ------------------------------------------------------------
void VisFemNode::refresh()
{
    double x, y, z;
    double dx, dy, dz;
    double scalefactor;


    if (m_beamModel!=NULL)
        scalefactor = m_beamModel->getScaleFactor();
    else
        scalefactor = 1.0;

    if (m_beamModel!=NULL)
        this->setSize(m_beamModel->getNodeSize());

    if (m_femNode!=NULL)
    {
        m_femNode->getCoord(x, y, z);
        dx = m_femNode->getValue(0);
        dy = m_femNode->getValue(1);
        dz = m_femNode->getValue(2);
    }
    else
    {
        x = y = z = 0.0;
        dx = dy = dz = 0.0;
    }

    if (m_beamModel!=NULL)
    {
        if (m_beamModel->getNodeType() == IVF_NODE_DISPLACEMENT)
        {
            if (m_beamModel->getResultType() != IVF_BEAM_NO_RESULT)
                this->setState(ivf::Shape::OS_OFF);
            else
                this->setState(ivf::Shape::OS_ON);
            
            ivf::Shape::setPosition(x + dx*scalefactor, y + dy*scalefactor,  z + dz*scalefactor);
        }
        else
        {
            this->setSize(m_beamModel->getNodeSize());
            ivf::Shape::setPosition(x, y, z);
        }
    }
}

void VisFemNode::doCreateGeometry()
{
    if (m_directRefresh)
        this->refresh();
    ivf::Node::doCreateGeometry();
}

void VisFemNode::setDirectRefresh(bool flag)
{
    m_directRefresh = flag;
}

void VisFemNode::getDisplacedPosition(double &x, double &y, double &z)
{
    double dx, dy, dz;
    double scalefactor;

    if (m_beamModel!=NULL)
        scalefactor = m_beamModel->getScaleFactor();
    else
        scalefactor = 1.0;

    if (m_femNode!=NULL)
    {
        m_femNode->getCoord(x, y, z);
        dx = m_femNode->getValue(0);
        dy = m_femNode->getValue(1);
        dz = m_femNode->getValue(2);
    }
    else
    {
        x = y = z = 0.0;
        dx = dy = dz = 0.0;
    }

    x = x + dx*scalefactor;
    y = y + dy*scalefactor;
    z = z + dz*scalefactor;
}

void VisFemNode::setBeamModel(VisBeamModel*model)
{
    m_beamModel = model;
}
