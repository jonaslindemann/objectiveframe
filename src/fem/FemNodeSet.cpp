// Implementation of: public class CFemNodeSet

#include "FemNodeSet.h"

// ------------------------------------------------------------
FemNodeSet::FemNodeSet ()
    :FemObject()
{
}

// ------------------------------------------------------------
FemNodeSet::~FemNodeSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemNodeSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_nodes.size(); i++)
        out << m_nodes[i];
    out << endl;
}

// ------------------------------------------------------------
void FemNodeSet::addNode(FemNode *node)
{
    //node->addReference();
    m_nodes.push_back(node);
}

// ------------------------------------------------------------
FemNode* FemNodeSet::getNode(long i)
{
    if ( (i>=0)&&(i<(long)m_nodes.size()) )
        return m_nodes[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool FemNodeSet::deleteNode(long i)
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
FemNode* FemNodeSet::removeNode(long i)
{
    if ( (i>=0)&&(i<(long)m_nodes.size()) )
    {
        if (m_nodes[i]->getRefCount()==1)
        {
            FemNode* node = m_nodes[i];
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
bool FemNodeSet::removeNode(FemNode* node)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        if (node==m_nodes[i])
            return this->deleteNode(i);
    }
    return false;
}
// ------------------------------------------------------------
void FemNodeSet::deleteAll()
{
    m_nodes.clear();
}

// ------------------------------------------------------------
void FemNodeSet::clear()
{
    m_nodes.clear();
}

// ------------------------------------------------------------
void FemNodeSet::clearNodeValues()
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        FemNode* node = m_nodes[i];
        node->clearValues();
    }
}

// ------------------------------------------------------------
size_t FemNodeSet::getSize()
{
    return m_nodes.size();
}

// ------------------------------------------------------------
long FemNodeSet::enumerateNodes(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
        m_nodes[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
long FemNodeSet::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_nodes.size(); i++)
        count = m_nodes[i]->enumerateDofs(count);
    return count;
}

// ------------------------------------------------------------
void FemNodeSet::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << m_nodes.size() << endl;
    for (unsigned int i=0; i<m_nodes.size(); i++)
    {
        FemNode* node = m_nodes[i];
        node->saveToStream(out);
    }
}

// ------------------------------------------------------------
json FemNodeSet::toJSON()
{
    json j = FemObject::toJSON();

    json nodeList;

    for (auto& node : m_nodes)
        nodeList.push_back(node->toJSON());

    j["nodeset"] = nodeList;

    return j;
}

// ------------------------------------------------------------
void FemNodeSet::readFromStream(std::istream &in)
{
    long nNodes;

    FemObject::readFromStream(in);
    deleteAll();
    in >> nNodes;
    for (int i=0; i<nNodes; i++)
    {
        FemNodePtr node = new FemNode();
        node->readFromStream(in);
        m_nodes.push_back(node);
    }
}

