// Implementation of: public class CFemLoadSet

#include "FemLoadSet.h"
#include "FemNodeLoad.h"
#include "FemElementLoad.h"

// ------------------------------------------------------------
FemLoadSet::FemLoadSet ()
    :FemObject()
{
}

// ------------------------------------------------------------
FemLoadSet::~FemLoadSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemLoadSet::print(std::ostream &out)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
        out << m_loads[i];
    out << endl;
}

// ------------------------------------------------------------
void FemLoadSet::addLoad(FemLoad *load)
{
    load->addReference();
    m_loads.push_back(load);
}

// ------------------------------------------------------------
FemLoad* FemLoadSet::getLoad(long i)
{
    if ( (i>=0)&&(i<(long)m_loads.size()) )
        return m_loads[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool FemLoadSet::deleteLoad(long i)
{
    std::vector<FemLoad*>::iterator p = m_loads.begin();

    if ( (i>=0)&&(i<(long)m_loads.size()) )
    {
        FemLoad* load = m_loads[i];

        load->deleteReference();
        if (!load->isReferenced())
        {
            p += i;
            m_loads.erase(p);
            delete load;
            return true;
        }
        load->addReference();
    }
    return false;
}

// ------------------------------------------------------------
FemLoad* FemLoadSet::removeLoad(long i)
{
    std::vector<FemLoad*>::iterator p = m_loads.begin();

    if ( (i>=0)&&(i<(long)m_loads.size()) )
    {
        FemLoad* load = m_loads[i];
        load->deleteReference();
        p += i;
        m_loads.erase(p);
        return load;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void FemLoadSet::deleteAll()
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        FemLoad* load = m_loads[i];
        load->deleteReference();
        if (!load->isReferenced())
            delete load;
    }
    m_loads.clear();
}

// ------------------------------------------------------------
void FemLoadSet::clear()
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        FemLoad* load = m_loads[i];
        load->deleteReference();
    }
    m_loads.clear();
}

// ------------------------------------------------------------
long FemLoadSet::enumerateLoads(long count)
{
    for (long i=0; i<(long)m_loads.size(); i++)
        m_loads[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
size_t FemLoadSet::getSize()
{
    return m_loads.size();
}

// ------------------------------------------------------------
void FemLoadSet::saveToStream(std::ostream &out)
{
    FemObject::saveToStream(out);
    out << m_loads.size() << endl;
    for (unsigned int i=0; i<m_loads.size(); i++)
        m_loads[i]->saveToStream(out);
}

// ------------------------------------------------------------
void FemLoadSet::readFromStream(std::istream &in)
{
    long nLoads;
    FemObject::readFromStream(in);
    in >> nLoads;
    deleteAll();
    for (int i=0; i<nLoads; i++)
    {
        FemLoad* load = createLoad();
        load->addReference();
        load->readFromStream(in);
        m_loads.push_back(load);
    }
}

// ------------------------------------------------------------
void FemLoadSet::connectNodes(FemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        FemLoad* load = m_loads[i];
        if (load->isClass("FemNodeLoad"))
        {
            FemNodeLoad* nodeLoad = (FemNodeLoad*) load;
            nodeLoad->clearNodes();
            for (unsigned int j=0; j<nodeLoad->getNodeIndexSize(); j++)
            {
                FemNode* node =
                    nodes->getNode(nodeLoad->getNodeIndex(j)-1);
                nodeLoad->addNode(node);
            }
        }
    }
}

// ------------------------------------------------------------
FemLoad* FemLoadSet::createLoad()
{
    return new FemLoad();
}

// ------------------------------------------------------------
void FemLoadSet::connectElements(FemElementSet *elements)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        FemLoad* load = m_loads[i];
        if (load->isClass("FemElementLoad"))
        {
            FemElementLoad* elementLoad = (FemElementLoad*) load;
            elementLoad->clearElements();
            for (unsigned int j=0; j<elementLoad->getElementIndexSize(); j++)
            {
                FemElement* element =
                    elements->getElement(elementLoad->getElementIndex(j)-1);
                elementLoad->addElement(element);
            }
        }
    }
}

// ------------------------------------------------------------
bool FemLoadSet::removeLoad(FemLoad *load)
{
    std::vector<FemLoad*>::iterator p = m_loads.begin();

    while ( (*p!=load)&&(p!=m_loads.end()) )
        p++;

    if (p!=m_loads.end())
    {
        FemLoad* l = *p;
        delete l;
        m_loads.erase(p);
        return true;
    }
    else
        return false;
}
