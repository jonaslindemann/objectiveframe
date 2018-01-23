// Implementation of: public class CFemNodeLoad

#include "FemNodeLoad.h"

// ------------------------------------------------------------
CFemNodeLoad::CFemNodeLoad ()
    :CFemLoad()
{
    m_direction[0] = 0.0;
    m_direction[1] = 0.0;
    m_direction[2] = 1.0;
}

// ------------------------------------------------------------
CFemNodeLoad::~CFemNodeLoad ()
{
    this->clearNodes();
}

// ------------------------------------------------------------
void CFemNodeLoad::print(ostream &out)
{

}

// ------------------------------------------------------------
void CFemNodeLoad::saveToStream(std::ostream &out)
{
    unsigned int i;
    CFemLoad::saveToStream(out);
    out << m_nodes.size() << endl;
    for (i=0; i<m_nodes.size(); i++)
        out << m_nodes[i]->getNumber() << endl;
    out << m_direction[0] << " ";
    out << m_direction[1] << " ";
    out << m_direction[2] << " ";
    out << endl;
}

// ------------------------------------------------------------
void CFemNodeLoad::readFromStream(std::istream &in)
{
    int nNodes, i;
    long idx;
    CFemLoad::readFromStream(in);
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
void CFemNodeLoad::setDirection(double ex, double ey, double ez)
{
    m_direction[0] = ex;
    m_direction[1] = ey;
    m_direction[2] = ez;
}

// ------------------------------------------------------------
void CFemNodeLoad::getDirection(double &ex, double &ey, double &ez)
{
    ex = m_direction[0];
    ey = m_direction[1];
    ez = m_direction[2];
}

// ------------------------------------------------------------
void CFemNodeLoad::setDirection(double *v)
{
    m_direction[0] = v[0];
    m_direction[1] = v[1];
    m_direction[2] = v[2];
}

// ------------------------------------------------------------
void CFemNodeLoad::getDirection(double *v)
{
    v[0] = m_direction[0];
    v[1] = m_direction[1];
    v[2] = m_direction[2];
}

// ------------------------------------------------------------
void CFemNodeLoad::addNode(CFemNode *node)
{
    node->addReference();
    m_nodes.push_back(node);
}

// ------------------------------------------------------------
void CFemNodeLoad::clearNodes()
{
    unsigned int i;

    for (i=0; i<m_nodes.size(); i++)
        m_nodes[i]->deleteReference();

    m_nodes.clear();
}

// ------------------------------------------------------------
bool CFemNodeLoad::removeNode(CFemNode *node)
{
    std::vector<CFemNode*>::iterator p = m_nodes.begin();

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
CFemNode* CFemNodeLoad::getNode(unsigned int idx)
{
    if (idx<m_nodes.size())
        return m_nodes[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
long CFemNodeLoad::getNodeIndex(unsigned int idx)
{
    if (idx<m_nodeIndex.size())
        return m_nodeIndex[idx];
    else
        return -1;
}

// ------------------------------------------------------------
unsigned int CFemNodeLoad::getNodeIndexSize()
{
    return m_nodeIndex.size();
}

// ------------------------------------------------------------
unsigned int CFemNodeLoad::getNodeSize()
{
    return m_nodes.size();
}
