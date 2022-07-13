// Implementation of: public class CFemElementSet

#include "FemElementSet.h"

// ------------------------------------------------------------
FemElementSet::FemElementSet ()
    :FemObject()
{
}

// ------------------------------------------------------------
FemElementSet::~FemElementSet ()
{
    deleteAll();
}

// ------------------------------------------------------------
void FemElementSet::print(std::ostream &out)
{
    using namespace std;
    for (unsigned int i=0; i<m_elements.size(); i++)
        out << m_elements[i];
    out << endl;
}

// ------------------------------------------------------------
void FemElementSet::addElement(FemElement *element)
{
    m_elements.push_back(FemElementPtr(element));
}

// ------------------------------------------------------------
FemElement* FemElementSet::getElement(long i)
{
    if ( (i>=0)&&(i<(long)m_elements.size()) )
        return m_elements[i];
    else
        return NULL;
}

// ------------------------------------------------------------
bool FemElementSet::deleteElement(long i)
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
FemElement* FemElementSet::removeElement(long i)
{
    if ( (i>=0)&&(i<(long)m_elements.size()) )
    {
        if (m_elements[i]->getRefCount()==1)
        {
            FemElement* element = m_elements[i];
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
void FemElementSet::deleteAll()
{
    m_elements.clear();
}

// ------------------------------------------------------------
void FemElementSet::clear()
{
    m_elements.clear();
}

// ------------------------------------------------------------
long FemElementSet::enumerateElements(long count)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
        m_elements[i]->setNumber(count++);
    return count;
}

// ------------------------------------------------------------
size_t FemElementSet::getSize()
{
    return m_elements.size();
}

// ------------------------------------------------------------
void FemElementSet::saveToStream(std::ostream &out)
{
    using namespace std;
    FemObject::saveToStream(out);
    out << m_elements.size() << endl;
    for (unsigned int i=0; i<m_elements.size(); i++)
        m_elements[i]->saveToStream(out);
}

// ------------------------------------------------------------
void FemElementSet::readFromStream(std::istream &in)
{
    long nElements;
    FemObject::readFromStream(in);
    in >> nElements;
    deleteAll();
    for (int i=0; i<nElements; i++)
    {
        FemElementPtr element = createElement();
        element->readFromStream(in);
        m_elements.push_back(element);
    }
}

// ------------------------------------------------------------
void FemElementSet::connectNodes(FemNodeSet *nodes)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        FemElement* element = m_elements[i];
        for (unsigned int j=0; j<element->getIndexSize(); j++)
            element->addNode(nodes->getNode(element->getElementIndex(j)-1));
    }
}

// ------------------------------------------------------------
FemElement* FemElementSet::createElement()
{
    return new FemElement();
}

// ------------------------------------------------------------
bool FemElementSet::removeElement(FemElement *element)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        if (element==m_elements[i])
            return this->deleteElement(i);
    }
    return false;
}

// ------------------------------------------------------------
long FemElementSet::enumerateDofs(long count)
{
    for (unsigned int i=0; i<m_elements.size(); i++)
        count = m_elements[i]->enumerateDofs(count);
    return count;
}
