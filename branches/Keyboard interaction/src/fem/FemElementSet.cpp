// Implementation of: public class CFemElementSet

#include "FemElementSet.h"

// ------------------------------------------------------------
CFemElementSet::CFemElementSet ()
    :CFemObject()
{
}

// ------------------------------------------------------------
CFemElementSet::~CFemElementSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void CFemElementSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_elements.size(); i++)
        out << m_elements[i];
    out << endl;
}

// ------------------------------------------------------------
void CFemElementSet::addElement(CFemElement *element)
{
    m_elements.push_back(CFemElementPtr(element));
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::getElement(long i)
{
    if ( (i>=0)&&(i<(long)m_elements.size()) )
        return m_elements[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool CFemElementSet::deleteElement(long i)
{
    if ( (i>=0)&&(i<(long)m_elements.size()) )
    {
        if (m_elements[i]->getRefCount()==1)
        {
            m_elements.erase(m_elements.begin()+i);
            return true;
        }
        else
            return false;
    }
    return false;
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::removeElement(long i)
{
    if ( (i>=0)&&(i<(long)m_elements.size()) )
    {
        if (m_elements[i]->getRefCount()==1)
        {
            CFemElement* element = m_elements[i];
            element->addReference();
            m_elements.erase(m_elements.begin()+i);
            element->deleteReference();
            return element;
        }
        else
            return NULL;
    }
    else
        return NULL;
}

// ------------------------------------------------------------
void CFemElementSet::deleteAll()
{
    m_elements.clear();
}

// ------------------------------------------------------------
void CFemElementSet::clear()
{
    m_elements.clear();
}

// ------------------------------------------------------------
long CFemElementSet::enumerateElements(long count)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
        m_elements[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
long CFemElementSet::getSize()
{
    return m_elements.size();
}

// ------------------------------------------------------------
void CFemElementSet::saveToStream(std::ostream &out)
{
    using namespace std;
    CFemObject::saveToStream(out);
    out << m_elements.size() << endl;
    for (unsigned int i=0; i<m_elements.size(); i++)
        m_elements[i]->saveToStream(out);
}

// ------------------------------------------------------------
void CFemElementSet::readFromStream(std::istream &in)
{
    long nElements;
    CFemObject::readFromStream(in);
    in >> nElements;
    deleteAll();
    for (int i=0; i<nElements; i++)
    {
        CFemElementPtr element = createElement();
        element->readFromStream(in);
        m_elements.push_back(element);
    }
}

// ------------------------------------------------------------
void CFemElementSet::connectNodes(CFemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        CFemElement* element = m_elements[i];
        for (unsigned int j=0; j<element->getIndexSize(); j++)
            element->addNode(nodes->getNode(element->getElementIndex(j)-1));
    }
}

// ------------------------------------------------------------
CFemElement* CFemElementSet::createElement()
{
    return new CFemElement();
}

// ------------------------------------------------------------
bool CFemElementSet::removeElement(CFemElement *element)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        if (element==m_elements[i])
            return this->deleteElement(i);
    }
    return false;
}

// ------------------------------------------------------------
long CFemElementSet::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
        count = m_elements[i]->enumerateDofs(count);
    return count;
}
