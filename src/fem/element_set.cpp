#include <ofem/element_set.h>

using namespace ofem;

ElementSet::ElementSet()
    : Base()
{
}

ElementSet::~ElementSet()
{
    deleteAll();
}

void ElementSet::print(std::ostream& out)
{
    using namespace std;
    for (unsigned int i = 0; i < m_elements.size(); i++)
        out << m_elements[i];
    out << endl;
}

void ElementSet::addElement(Element* element)
{
    m_elements.push_back(ElementPtr(element));
}

Element* ElementSet::getElement(long i)
{
    if ((i >= 0) && (i < (long)m_elements.size()))
        return m_elements[i].get();
    else
        return NULL;
}

bool ElementSet::deleteElement(long i)
{
    if ((i >= 0) && (i < (long)m_elements.size()))
    {
        if (m_elements[i]->getRefCount() == 1)
        {
            m_elements.erase(m_elements.begin() + i);
            return true;
        }
        else
            return false;
    }
    return false;
}

bool ElementSet::removeElement(long i)
{
    if ((i >= 0) && (i < (long)m_elements.size()))
    {
        if (m_elements[i]->getRefCount() == 1)
        {
            ElementPtr element = m_elements[i];
            m_elements.erase(m_elements.begin() + i);
            return true;
        }
        else
            return false;
    }
    else
        return false;
}

void ElementSet::deleteAll()
{
    m_elements.clear();
}

void ElementSet::clear()
{
    m_elements.clear();
}

long ElementSet::enumerateElements(long count)
{
    for (unsigned int i = 0; i < m_elements.size(); i++)
        m_elements[i]->setNumber(count++);
    return count;
}

size_t ElementSet::getSize()
{
    return m_elements.size();
}

void ElementSet::saveToStream(std::ostream& out)
{
    using namespace std;
    Base::saveToStream(out);
    out << m_elements.size() << endl;
    for (unsigned int i = 0; i < m_elements.size(); i++)
        m_elements[i]->saveToStream(out);
}

void ElementSet::readFromStream(std::istream& in)
{
    long nElements;
    Base::readFromStream(in);
    in >> nElements;
    deleteAll();
    for (int i = 0; i < nElements; i++)
    {
        ElementPtr element = ElementPtr(createElement());
        element->readFromStream(in);
        m_elements.push_back(element);
    }
}

void ElementSet::connectNodes(NodeSet* nodes)
{
    for (unsigned int i = 0; i < m_elements.size(); i++)
    {
        ElementPtr element = m_elements[i];
        for (unsigned int j = 0; j < element->getIndexSize(); j++)
            element->addNode(nodes->getNode(element->getElementIndex(j) - 1));
    }
}

Element* ElementSet::createElement()
{
    return new Element();
}

bool ElementSet::removeElement(Element* element)
{
    for (unsigned int i = 0; i < m_elements.size(); i++)
    {
        if (element == m_elements[i].get())
            return this->deleteElement(i);
    }
    return false;
}

long ElementSet::enumerateDofs(long count)
{
    for (unsigned int i = 0; i < m_elements.size(); i++)
        count = m_elements[i]->enumerateDofs(count);
    return count;
}
