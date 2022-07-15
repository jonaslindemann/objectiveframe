#include <vfem/node.h>

//using namespace ivf;
using namespace vfem;

#include <ivf/Node.h>

// ------------------------------------------------------------
Node::Node ()
    :ivf::Node()
{
    m_femNode = nullptr;
    m_directRefresh = false;

    m_beamModel = nullptr;

    this->setUseSelectShape(false);
    this->setType(Node::NT_SPHERE);
}

// ------------------------------------------------------------
Node::~Node ()
{
}

// ------------------------------------------------------------
void Node::setFemNode(ofem::Node *node)
{
    double x, y, z;
    m_femNode = node;
    m_femNode->getCoord(x, y, z);
    Shape::setPosition(x, y, z);
}

// ------------------------------------------------------------
ofem::Node* Node::getFemNode()
{
    return m_femNode;
}

// ------------------------------------------------------------
void Node::setPosition (const double x, const double y, const double z)
{
    if (m_femNode!=nullptr)
        m_femNode->setCoord(x, y, z);
    ivf::Shape::setPosition(x, y, z);
}

// ------------------------------------------------------------
void Node::setPosition (ivf::Shape* shape)
{
    if (m_femNode!=nullptr)
    {
        double x, y, z;
        shape->getPosition(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    ivf::Shape::setPosition(shape);
}

// ------------------------------------------------------------
void Node::setPositionVec (ivf::Vec3d* point)
{
    if (m_femNode!=nullptr)
    {
        double x, y, z;
        point->getComponents(x, y, z);
        m_femNode->setCoord(x, y, z);
    }
    ivf::Shape::setPosition(*point);
}

// ------------------------------------------------------------
void Node::refresh()
{
    double x, y, z;
    double dx, dy, dz;
    double scalefactor;


    if (m_beamModel!=nullptr)
        scalefactor = m_beamModel->getScaleFactor();
    else
        scalefactor = 1.0;

    if (m_beamModel!=nullptr)
        this->setSize(m_beamModel->getNodeSize());

    if (m_femNode!=nullptr)
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

    if (m_beamModel!=nullptr)
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

void Node::doCreateGeometry()
{
    if (m_directRefresh)
        this->refresh();
    ivf::Node::doCreateGeometry();
}

void Node::setDirectRefresh(bool flag)
{
    m_directRefresh = flag;
}

void Node::getDisplacedPosition(double &x, double &y, double &z)
{
    double dx, dy, dz;
    double scalefactor;

    if (m_beamModel!=nullptr)
        scalefactor = m_beamModel->getScaleFactor();
    else
        scalefactor = 1.0;

    if (m_femNode!=nullptr)
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

void Node::setBeamModel(BeamModel*model)
{
    m_beamModel = model;
}
