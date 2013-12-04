// Implementation of: public class CFemLoadSet

#include "FemLoadSet.h"
#include "FemNodeLoad.h"
#include "FemElementLoad.h"

// ------------------------------------------------------------
CFemLoadSet::CFemLoadSet ()
    :CFemObject()
{
}

// ------------------------------------------------------------
CFemLoadSet::~CFemLoadSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void CFemLoadSet::print(std::ostream &out)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
        out << m_loads[i];
    out << endl;
}

// ------------------------------------------------------------
void CFemLoadSet::addLoad(CFemLoad *load)
{
    load->addReference();
    m_loads.push_back(load);
}

// ------------------------------------------------------------
CFemLoad* CFemLoadSet::getLoad(long i)
{
    if ( (i>=0)&&(i<(long)m_loads.size()) )
        return m_loads[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool CFemLoadSet::deleteLoad(long i)
{
    std::vector<CFemLoad*>::iterator p = m_loads.begin();

    if ( (i>=0)&&(i<(long)m_loads.size()) )
    {
        CFemLoad* load = m_loads[i];

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
CFemLoad* CFemLoadSet::removeLoad(long i)
{
    std::vector<CFemLoad*>::iterator p = m_loads.begin();

    if ( (i>=0)&&(i<(long)m_loads.size()) )
    {
        CFemLoad* load = m_loads[i];
        load->deleteReference();
        p += i;
        m_loads.erase(p);
        return load;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void CFemLoadSet::deleteAll()
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        CFemLoad* load = m_loads[i];
        load->deleteReference();
        if (!load->isReferenced())
            delete load;
    }
    m_loads.clear();
}

// ------------------------------------------------------------
void CFemLoadSet::clear()
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        CFemLoad* load = m_loads[i];
        load->deleteReference();
    }
    m_loads.clear();
}

// ------------------------------------------------------------
long CFemLoadSet::enumerateLoads(long count)
{
    for (long i=0; i<(long)m_loads.size(); i++)
        m_loads[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
long CFemLoadSet::getSize()
{
    return m_loads.size();
}

// ------------------------------------------------------------
void CFemLoadSet::saveToStream(std::ostream &out)
{
    CFemObject::saveToStream(out);
    out << m_loads.size() << endl;
    for (unsigned int i=0; i<m_loads.size(); i++)
        m_loads[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemLoadSet::readFromStream(std::istream &in)
{
    long nLoads;
    CFemObject::readFromStream(in);
    in >> nLoads;
    deleteAll();
    for (int i=0; i<nLoads; i++)
    {
        CFemLoad* load = createLoad();
        load->addReference();
        load->readFromStream(in);
        m_loads.push_back(load);
    }
}

// ------------------------------------------------------------
void CFemLoadSet::connectNodes(CFemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        CFemLoad* load = m_loads[i];
        if (load->isClass("CFemNodeLoad"))
        {
            CFemNodeLoad* nodeLoad = (CFemNodeLoad*) load;
            nodeLoad->clearNodes();
            for (unsigned int j=0; j<nodeLoad->getNodeIndexSize(); j++)
            {
                CFemNode* node =
                    nodes->getNode(nodeLoad->getNodeIndex(j)-1);
                nodeLoad->addNode(node);
            }
        }
    }
}

// ------------------------------------------------------------
CFemLoad* CFemLoadSet::createLoad()
{
    return new CFemLoad();
}

// ------------------------------------------------------------
void CFemLoadSet::connectElements(CFemElementSet *elements)
{
    for (unsigned int i=0; i<m_loads.size(); i++)
    {
        CFemLoad* load = m_loads[i];
        if (load->isClass("CFemElementLoad"))
        {
            CFemElementLoad* elementLoad = (CFemElementLoad*) load;
            elementLoad->clearElements();
            for (unsigned int j=0; j<elementLoad->getElementIndexSize(); j++)
            {
                CFemElement* element =
                    elements->getElement(elementLoad->getElementIndex(j)-1);
                elementLoad->addElement(element);
            }
        }
    }
}

// ------------------------------------------------------------
bool CFemLoadSet::removeLoad(CFemLoad *load)
{
    std::vector<CFemLoad*>::iterator p = m_loads.begin();

    while ( (*p!=load)&&(p!=m_loads.end()) )
        p++;

    if (p!=m_loads.end())
    {
        CFemLoad* l = *p;
        delete l;
        m_loads.erase(p);
        return true;
    }
    else
        return false;
}
