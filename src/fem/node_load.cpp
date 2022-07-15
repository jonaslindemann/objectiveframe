#include <ofem/node_load.h>

using namespace ofem;
using namespace std;

// ------------------------------------------------------------
NodeLoad::NodeLoad ()
    :Load()
{
    m_direction[0] = 0.0;
    m_direction[1] = 0.0;
    m_direction[2] = 1.0;
}

// ------------------------------------------------------------
NodeLoad::~NodeLoad ()
{
    this->clearNodes();
}

// ------------------------------------------------------------
void NodeLoad::print(ostream &out)
{

}

// ------------------------------------------------------------
void NodeLoad::saveToStream(std::ostream &out)
{
    unsigned int i;
    Load::saveToStream(out);
    out << m_nodes.size() << endl;
    for (i=0; i<m_nodes.size(); i++)
        out << m_nodes[i]->getNumber() << endl;
    out << m_direction[0] << " ";
    out << m_direction[1] << " ";
    out << m_direction[2] << " ";
    out << endl;
}

// ------------------------------------------------------------
void NodeLoad::readFromStream(std::istream &in)
{
    int nNodes, i;
    long idx;
    Load::readFromStream(in);
    in >> nNodes;
    for (i=0; i<nNodes; i++)
    {
        in >> idx;
        m_nodeIndex.push_back(idx);
    }
    in >> m_direction[0];
    in >> m_direction[1];
    in >> m_direction[2];
}

// ------------------------------------------------------------
void NodeLoad::setDirection(double ex, double ey, double ez)
{
    m_direction[0] = ex;
    m_direction[1] = ey;
    m_direction[2] = ez;
}

// ------------------------------------------------------------
void NodeLoad::getDirection(double &ex, double &ey, double &ez)
{
    ex = m_direction[0];
    ey = m_direction[1];
    ez = m_direction[2];
}

// ------------------------------------------------------------
void NodeLoad::setDirection(double *v)
{
    m_direction[0] = v[0];
    m_direction[1] = v[1];
    m_direction[2] = v[2];
}

// ------------------------------------------------------------
void NodeLoad::getDirection(double *v)
{
    v[0] = m_direction[0];
    v[1] = m_direction[1];
    v[2] = m_direction[2];
}

// ------------------------------------------------------------
void NodeLoad::addNode(Node *node)
{
    node->addReference();
    m_nodes.push_back(node);
}

// ------------------------------------------------------------
void NodeLoad::clearNodes()
{
    unsigned int i;

    for (i=0; i<m_nodes.size(); i++)
        m_nodes[i]->deleteReference();

    m_nodes.clear();
}

// ------------------------------------------------------------
bool NodeLoad::removeNode(Node *node)
{
    std::vector<Node*>::iterator p = m_nodes.begin();

    while ( (p!=m_nodes.end())&&(*p!=node) )
        p++;

	if (p != m_nodes.end())
	{
		if (*p == node)
		{
			node->deleteReference();
			m_nodes.erase(p);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

// ------------------------------------------------------------
Node* NodeLoad::getNode(unsigned int idx)
{
    if (idx<m_nodes.size())
        return m_nodes[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
long NodeLoad::getNodeIndex(unsigned int idx)
{
    if (idx<m_nodeIndex.size())
        return m_nodeIndex[idx];
    else
        return -1;
}

// ------------------------------------------------------------
size_t NodeLoad::getNodeIndexSize()
{
    return m_nodeIndex.size();
}

// ------------------------------------------------------------
size_t NodeLoad::getNodeSize()
{
    return m_nodes.size();
}
