#include <ofem/element_load.h>
#include <ofem/load_set.h>
#include <ofem/node_load.h>

using namespace ofem;
using namespace std;


LoadSet::LoadSet()
    : Base()
{
}


LoadSet::~LoadSet()
{
    deleteAll();
}


void LoadSet::print(std::ostream& out)
{
    for (unsigned int i = 0; i < m_loads.size(); i++)
        out << m_loads[i];
    out << endl;
}


void LoadSet::addLoad(Load* load)
{
    load->addReference();
    m_loads.push_back(load);
}


Load* LoadSet::getLoad(long i)
{
    if ((i >= 0) && (i < (long)m_loads.size()))
        return m_loads[i];
    else
        return NULL;
}


bool LoadSet::deleteLoad(long i)
{
    std::vector<Load*>::iterator p = m_loads.begin();

    if ((i >= 0) && (i < (long)m_loads.size()))
    {
        Load* load = m_loads[i];

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


Load* LoadSet::removeLoad(long i)
{
    std::vector<Load*>::iterator p = m_loads.begin();

    if ((i >= 0) && (i < (long)m_loads.size()))
    {
        Load* load = m_loads[i];
        load->deleteReference();
        p += i;
        m_loads.erase(p);
        return load;
    }
    else
        return NULL;
}


void LoadSet::deleteAll()
{
    for (unsigned int i = 0; i < m_loads.size(); i++)
    {
        Load* load = m_loads[i];
        load->deleteReference();
        if (!load->isReferenced())
            delete load;
    }
    m_loads.clear();
}


void LoadSet::clear()
{
    for (unsigned int i = 0; i < m_loads.size(); i++)
    {
        Load* load = m_loads[i];
        load->deleteReference();
    }
    m_loads.clear();
}


long LoadSet::enumerateLoads(long count)
{
    for (long i = 0; i < (long)m_loads.size(); i++)
        m_loads[i]->setNumber(count++);
    return count;
}


size_t LoadSet::getSize()
{
    return m_loads.size();
}


void LoadSet::saveToStream(std::ostream& out)
{
    Base::saveToStream(out);
    out << m_loads.size() << endl;
    for (unsigned int i = 0; i < m_loads.size(); i++)
        m_loads[i]->saveToStream(out);
}


void LoadSet::readFromStream(std::istream& in)
{
    long nLoads;
    Base::readFromStream(in);
    in >> nLoads;
    deleteAll();
    for (int i = 0; i < nLoads; i++)
    {
        Load* load = createLoad();
        load->addReference();
        load->readFromStream(in);
        m_loads.push_back(load);
    }
}


void LoadSet::connectNodes(NodeSet* nodes)
{
    for (unsigned int i = 0; i < m_loads.size(); i++)
    {
        Load* load = m_loads[i];
        if (load->isClass("NodeLoad"))
        {
            NodeLoad* nodeLoad = (NodeLoad*)load;
            nodeLoad->clearNodes();
            for (unsigned int j = 0; j < nodeLoad->getNodeIndexSize(); j++)
            {
                Node* node = nodes->getNode(nodeLoad->getNodeIndex(j) - 1);
                nodeLoad->addNode(node);
            }
        }
    }
}


Load* LoadSet::createLoad()
{
    return new Load();
}


void LoadSet::connectElements(ElementSet* elements)
{
    for (unsigned int i = 0; i < m_loads.size(); i++)
    {
        Load* load = m_loads[i];
        if (load->isClass("ElementLoad"))
        {
            ElementLoad* elementLoad = (ElementLoad*)load;
            elementLoad->clearElements();
            for (unsigned int j = 0; j < elementLoad->getElementIndexSize(); j++)
            {
                Element* element = elements->getElement(elementLoad->getElementIndex(j) - 1);
                elementLoad->addElement(element);
            }
        }
    }
}


bool LoadSet::removeLoad(Load* load)
{
    std::vector<Load*>::iterator p = m_loads.begin();

    while ((*p != load) && (p != m_loads.end()))
        p++;

    if (p != m_loads.end())
    {
        Load* l = *p;
        delete l;
        m_loads.erase(p);
        return true;
    }
    else
        return false;
}
