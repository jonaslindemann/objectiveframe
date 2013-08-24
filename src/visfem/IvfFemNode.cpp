// Implementation of: public class CIvfFemNode

#include "IvfFemNode.h"

// ------------------------------------------------------------
CIvfFemNode::CIvfFemNode ()
    :CIvfNode()
{
    m_femNode = NULL;
    m_directRefresh = false;

    m_beamModel = NULL;

    this->setUseSelectShape(false);
    this->setType(CIvfNode::NT_SPHERE);
}

// ------------------------------------------------------------
CIvfFemNode::~CIvfFemNode ()
{
}

// ------------------------------------------------------------
void CIvfFemNode::setFemNode(CFemNode *node)
{
    double x, y, z;
    m_femNode = node;
    m_femNode->getCoord(x, y, z);
    CIvfShape::setPosition(x, y, z);
}

// ------------------------------------------------------------
CFemNode* CIvfFemNode::getFemNode()
{
    return m_femNode;
}

// ------------------------------------------------------------
void CIvfFemNode::setPosition (const double x, const double y, const double z)
{
    if (m_femNode!=NULL)
        m_femNode->setCoord(x, y, z);
    CIvfShape::setPosition(x, y, z);
}

// ------------------------------------------------------------
void CIvfFemNode::setPosition (CIvfShape* shape)
{
    if (m_femNode!=NULL)
    {
        double x, y, z;
        shape->getPosition(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    CIvfShape::setPosition(shape);
}

// ------------------------------------------------------------
void CIvfFemNode::setPositionVec (CIvfVec3d* point)
{
    if (m_femNode!=NULL)
    {
        double x, y, z;
        point->getComponents(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    CIvfShape::setPosition(*point);
}

// ------------------------------------------------------------
void CIvfFemNode::refresh()
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
                this->setState(CIvfShape::OS_OFF);
            else
                this->setState(CIvfShape::OS_ON);
            
            CIvfShape::setPosition(x + dx*scalefactor, y + dy*scalefactor,  z + dz*scalefactor);
        }
        else
        {
            this->setSize(m_beamModel->getNodeSize());
            CIvfShape::setPosition(x, y, z);
        }
    }
}

void CIvfFemNode::doCreateGeometry()
{
    if (m_directRefresh)
        this->refresh();
    CIvfNode::doCreateGeometry();
}

void CIvfFemNode::setDirectRefresh(bool flag)
{
    m_directRefresh = flag;
}

void CIvfFemNode::getDisplacedPosition(double &x, double &y, double &z)
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

void CIvfFemNode::setBeamModel(CIvfBeamModel *model)
{
    m_beamModel = model;
}
