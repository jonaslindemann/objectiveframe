// Implementation of: public class CFemBCSet

#include "FemBCSet.h"

#include "FemNodeBC.h"

// ------------------------------------------------------------
FemBCSet::FemBCSet ()
    :FemObject()
{
}

// ------------------------------------------------------------
FemBCSet::~FemBCSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemBCSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_bcs.size(); i++)
        out << m_bcs[i];
    out << endl;
}

// ------------------------------------------------------------
json FemBCSet::toJSON()
{
    json j = FemObject::toJSON();

    json bcList;

    for (auto& bc : m_bcs)
        bcList.push_back(bc->toJSON());

    j["bcset"] = bcList;

    return j;
}

// ------------------------------------------------------------
void FemBCSet::addBC(FemBC *bc)
{
    m_bcs.push_back(FemBCPtr(bc));
}

// ------------------------------------------------------------
FemBC* FemBCSet::getBC(long i)
{
    if ( (i>=0)&&(i<(long)m_bcs.size()) )
        return m_bcs[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool FemBCSet::deleteBC(long i)
{
    std::vector<FemBCPtr>::iterator p = m_bcs.begin();

    if ( (i>=0)&&(i<(long)m_bcs.size()) )
    {
        p += i;
        m_bcs.erase(p);
        return true;
    }
    return false;
}

// ------------------------------------------------------------
FemBCPtr FemBCSet::removeBC(long i)
{
    std::vector<FemBCPtr>::iterator p = m_bcs.begin();

    if ( (i>=0)&&(i<(long)m_bcs.size()) )
    {
        FemBCPtr bc = m_bcs[i];
        p += i;
        m_bcs.erase(p);
        return bc;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void FemBCSet::deleteAll()
{
    m_bcs.clear();
}

// ------------------------------------------------------------
void FemBCSet::clear()
{
    m_bcs.clear();
}

// ------------------------------------------------------------
long FemBCSet::enumerateBCs(long count)
{
    for (auto i=0; i<m_bcs.size(); i++)
        m_bcs[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
size_t FemBCSet::getSize()
{
    return m_bcs.size();
}

// ------------------------------------------------------------
void FemBCSet::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << m_bcs.size() << endl;
    for (auto i=0; i<m_bcs.size(); i++)
        m_bcs[i]->saveToStream(out);
}

// ------------------------------------------------------------
void FemBCSet::readFromStream(std::istream &in)
{
    long nBCs;
    FemObject::readFromStream(in);
    in >> nBCs;
    deleteAll();
    for (auto i=0; i<nBCs; i++)
    {
        FemBC* bc = createBC();
        bc->addReference();
        bc->readFromStream(in);
        m_bcs.push_back(bc);
    }
}

// ------------------------------------------------------------
void FemBCSet::connectNodes(FemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_bcs.size(); i++)
    {
        FemBC* bc = m_bcs[i];
        if (bc->isClass("FemNodeBC"))
        {
            auto nodeBC = dynamic_cast<FemNodeBC*>(bc);
            //CFemNodeBC* nodeBC = (CFemNodeBC*) bc;
            nodeBC->clearNodes();
            for (auto j=0; j<nodeBC->getNodeIndexSize(); j++)
            {
                FemNode* node =
                    nodes->getNode(nodeBC->getNodeIndex(j)-1);
                nodeBC->addNode(node);
            }
        }
    }
}

// ------------------------------------------------------------
FemBC* FemBCSet::createBC()
{
    return new FemBC();
}

// ------------------------------------------------------------
bool FemBCSet::removeBC(FemBC *bc)
{
    auto p = m_bcs.begin();

    while ( (*p!=bc)&&(p!=m_bcs.end()) )
        p++;

    if (p!=m_bcs.end())
    {
		if (!bc->isReadOnly())
		{
			m_bcs.erase(p);
			return true;
		}
		else
			return false;
    }
    else
        return false;
}
