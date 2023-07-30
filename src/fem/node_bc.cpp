#include <ofem/node_bc.h>

using namespace ofem;


NodeBC::NodeBC()
{
    int i;
    for (i = 0; i < 6; i++)
    {
        m_prescribedDof[i] = true;
        m_prescribedValues[i] = 0.0;
    }
}


NodeBC::~NodeBC()
{
    this->clearNodes();
}


void NodeBC::print(std::ostream& out)
{
}


json_nl NodeBC::toJson()
{
    json_nl j;

    json_nl nodeNumbers;

    for (auto& node : m_nodes)
        nodeNumbers.push_back(node->getNumber());

    j["nodes"] = nodeNumbers;
    j["prescribed_dofs"] = m_prescribedDof;
    j["prescribed_values"] = m_prescribedValues;

    return j;
}


void NodeBC::saveToStream(std::ostream& out)
{
    using namespace std;
    unsigned int i;
    // CFemBC::saveToStream(out);
    out << m_nodes.size() << endl;
    for (i = 0; i < m_nodes.size(); i++)
        out << m_nodes[i]->getNumber() << endl;
    out << endl;
    for (i = 0; i < 6; i++)
    {
        if (m_prescribedDof[i])
            out << 1 << " " << m_prescribedValues[i] << endl;
        else
            out << 0 << " " << 0.0 << endl;
    }
}


void NodeBC::readFromStream(std::istream& in)
{
    int nNodes, i;
    long idx;
    int flag;
    double value;
    // CFemBC::readFromStream(in);
    in >> nNodes;
    for (i = 0; i < nNodes; i++)
    {
        in >> idx;
        m_nodeIndex.push_back(idx);
    }
    for (i = 0; i < 6; i++)
    {
        in >> flag >> value;
        if (flag == 1)
            prescribe(i + 1, value);
        else
            unprescribe(i + 1);
    }
}


void NodeBC::addNode(Node* node)
{
    m_nodes.emplace_back(NodePtr(node));
}


void NodeBC::clearNodes()
{
    m_nodes.clear();
}


bool NodeBC::removeNode(Node* node)
{
    auto p = m_nodes.begin();

    while ((p != m_nodes.end()) && ((*p).get() != node))
        p++;

    if (p != m_nodes.end())
    {
        if ((*p).get() == node)
        {
            m_nodes.erase(p);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}


Node* NodeBC::getNode(unsigned int idx)
{
    if (idx < m_nodes.size())
        return m_nodes[idx].get();
    else
        return NULL;
}


long NodeBC::getNodeIndex(unsigned int idx)
{
    if (idx < m_nodeIndex.size())
        return m_nodeIndex[idx];
    else
        return -1;
}


size_t NodeBC::getNodeIndexSize()
{
    return m_nodeIndex.size();
}


size_t NodeBC::getNodeSize()
{
    return m_nodes.size();
}


void NodeBC::prescribe(int dof, double value)
{
    if ((dof >= 1) && (dof <= 6))
    {
        m_prescribedDof[dof - 1] = true;
        m_prescribedValues[dof - 1] = value;
    }
}

void NodeBC::prescribePos(double value)
{
    for (int i = 1; i <= 3; i++)
        this->prescribe(i, value);
}

void NodeBC::prescribeRot(double value)
{
    for (int i = 4; i <= 6; i++)
        this->prescribe(i, value);
}

void NodeBC::fixed()
{
    this->release();
    this->prescribePos(0.0);
    this->prescribeRot(0.0);
}

void NodeBC::fixedPosition()
{
    this->release();
    this->prescribePos(0.0);
}

void NodeBC::release()
{
    for (int i = 1; i <= 6; i++)
        this->unprescribe(i);
}


void NodeBC::unprescribe(int dof)
{
    if ((dof >= 1) && (dof <= 6))
    {
        m_prescribedDof[dof - 1] = false;
        m_prescribedValues[dof - 1] = 0.0;
    }
}


bool NodeBC::isPrescribed(int dof)
{
    if ((dof >= 1) && (dof <= 6))
        return m_prescribedDof[dof - 1];
    else
        return false;
}


double NodeBC::getPrescribedValue(int dof)
{
    if ((dof >= 1) && (dof <= 6))
        return m_prescribedValues[dof - 1];
    else
        return 0.0;
}

bool* NodeBC::getPrescribedArr()
{
    return m_prescribedDof;
}

double* NodeBC::getPrescribedValueArr()
{
    return m_prescribedValues;
}
