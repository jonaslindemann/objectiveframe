// Implementation of: public class CFemBCSet

#include "FemBCSet.h"

#include "FemNodeBC.h"

// ------------------------------------------------------------
CFemBCSet::CFemBCSet ()
    :CFemObject()
{
}

// ------------------------------------------------------------
CFemBCSet::~CFemBCSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void CFemBCSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_bcs.size(); i++)
        out << m_bcs[i];
    out << endl;
}

// ------------------------------------------------------------
void CFemBCSet::addBC(CFemBC *bc)
{
    m_bcs.push_back(CFemBCPtr(bc));
}

// ------------------------------------------------------------
CFemBC* CFemBCSet::getBC(long i)
{
    if ( (i>=0)&&(i<(long)m_bcs.size()) )
        return m_bcs[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool CFemBCSet::deleteBC(long i)
{
    std::vector<CFemBCPtr>::iterator p = m_bcs.begin();

    if ( (i>=0)&&(i<(long)m_bcs.size()) )
    {
        p += i;
        m_bcs.erase(p);
        return true;
    }
    return false;
}

// ------------------------------------------------------------
CFemBCPtr CFemBCSet::removeBC(long i)
{
    std::vector<CFemBCPtr>::iterator p = m_bcs.begin();

    if ( (i>=0)&&(i<(long)m_bcs.size()) )
    {
        CFemBCPtr bc = m_bcs[i];
        p += i;
        m_bcs.erase(p);
        return bc;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void CFemBCSet::deleteAll()
{
    m_bcs.clear();
}

// ------------------------------------------------------------
void CFemBCSet::clear()
{
    m_bcs.clear();
}

// ------------------------------------------------------------
long CFemBCSet::enumerateBCs(long count)
{
    for (int i=0; i<(long)m_bcs.size(); i++)
        m_bcs[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
long CFemBCSet::getSize()
{
    return m_bcs.size();
}

// ------------------------------------------------------------
void CFemBCSet::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
    out << m_bcs.size() << endl;
    for (int i=0; i<(long)m_bcs.size(); i++)
        m_bcs[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemBCSet::readFromStream(std::istream &in)
{
    long nBCs;
    CFemObject::readFromStream(in);
    in >> nBCs;
    deleteAll();
    for (int i=0; i<nBCs; i++)
    {
        CFemBC* bc = createBC();
        bc->addReference();
        bc->readFromStream(in);
        m_bcs.push_back(bc);
    }
}

// ------------------------------------------------------------
void CFemBCSet::connectNodes(CFemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_bcs.size(); i++)
    {
        CFemBC* bc = m_bcs[i];
        if (bc->isClass("CFemNodeBC"))
        {
            CFemNodeBC* nodeBC = (CFemNodeBC*) bc;
            nodeBC->clearNodes();
            for (int j=0; j<nodeBC->getNodeIndexSize(); j++)
            {
                CFemNode* node =
                    nodes->getNode(nodeBC->getNodeIndex(j)-1);
                nodeBC->addNode(node);
            }
        }
    }
}

// ------------------------------------------------------------
CFemBC* CFemBCSet::createBC()
{
    return new CFemBC();
}

// ------------------------------------------------------------
bool CFemBCSet::removeBC(CFemBC *bc)
{
    std::vector<CFemBCPtr>::iterator p = m_bcs.begin();

    while ( (*p!=bc)&&(p!=m_bcs.end()) )
        p++;

    if (p!=m_bcs.end())
    {
        m_bcs.erase(p);
        return true;
    }
    else
        return false;
}
