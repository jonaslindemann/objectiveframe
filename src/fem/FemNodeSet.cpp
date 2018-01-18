// Implementation of: public class CFemNodeSet

#include "FemNodeSet.h"

// ------------------------------------------------------------
CFemNodeSet::CFemNodeSet ()
    :CFemObject()
{
}

// ------------------------------------------------------------
CFemNodeSet::~CFemNodeSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void CFemNodeSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_nodes.size(); i++)
        out << m_nodes[i];
    out << endl;
}

// ------------------------------------------------------------
void CFemNodeSet::addNode(CFemNode *node)
{
    //node->addReference();
    m_nodes.push_back(node);
}

// ------------------------------------------------------------
CFemNode* CFemNodeSet::getNode(long i)
{
    if ( (i>=0)&&(i<(long)m_nodes.size()) )
        return m_nodes[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool CFemNodeSet::deleteNode(long i)
{
    if ( (i>=0)&&(i<(long)m_nodes.size()) )
    {
        if (m_nodes[i]->getRefCount()==1)
        {
            m_nodes.erase(m_nodes.begin()+i);
            return true;
        }
        else
            return false;
    }
    return false;
}

// ------------------------------------------------------------
CFemNode* CFemNodeSet::removeNode(long i)
{
    if ( (i>=0)&&(i<(long)m_nodes.size()) )
    {
        if (m_nodes[i]->getRefCount()==1)
        {
            CFemNode* node = m_nodes[i];
            node->addReference();
            m_nodes.erase(m_nodes.begin()+i);
            node->deleteReference();
            return node;
        }
        else
            return 0;
    }
    else
        return 0;
}

// ------------------------------------------------------------
bool CFemNodeSet::removeNode(CFemNode* node)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        if (node==m_nodes[i])
            return this->deleteNode(i);
    }
    return false;
}
// ------------------------------------------------------------
void CFemNodeSet::deleteAll()
{
    m_nodes.clear();
}

// ------------------------------------------------------------
void CFemNodeSet::clear()
{
    m_nodes.clear();
}

// ------------------------------------------------------------
void CFemNodeSet::clearNodeValues()
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        CFemNode* node = m_nodes[i];
        node->clearValues();
    }
}

// ------------------------------------------------------------
long CFemNodeSet::getSize()
{
    return m_nodes.size();
}

// ------------------------------------------------------------
long CFemNodeSet::enumerateNodes(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
        m_nodes[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
long CFemNodeSet::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
        count = m_nodes[i]->enumerateDofs(count);
    return count;
}

// ------------------------------------------------------------
void CFemNodeSet::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
    out << m_nodes.size() << endl;
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        CFemNode* node = m_nodes[i];
        node->saveToStream(out);
    }
}

// ------------------------------------------------------------
void CFemNodeSet::readFromStream(std::istream &in)
{
    long nNodes;

    CFemObject::readFromStream(in);
    deleteAll();
    in >> nNodes;
    for (int i=0; i<nNodes; i++)
    {
        CFemNodePtr node = new CFemNode();
        node->readFromStream(in);
        m_nodes.push_back(node);
    }
}

