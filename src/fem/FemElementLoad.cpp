// Implementation of: public class CFemElementLoad

#include "FemElementLoad.h"

// ------------------------------------------------------------
FemElementLoad::FemElementLoad ()
    :FemLoad()
{
    m_localDirection[0] = 0.0;
    m_localDirection[1] = 0.0;
    m_localDirection[2] = 1.0;
}

// ------------------------------------------------------------
FemElementLoad::~FemElementLoad ()
{
    this->clearElements();
}

// ------------------------------------------------------------
void FemElementLoad::print(ostream &out)
{

}

// ------------------------------------------------------------
void FemElementLoad::saveToStream(std::ostream &out)
{
    FemLoad::saveToStream(out);
    out << m_localDirection[0] << " ";
    out << m_localDirection[1] << " ";
    out << m_localDirection[2] << " ";
    out << endl;
    out << m_elements.size() << " ";
    for (unsigned int i=0; i<m_elements.size(); i++)
        out << m_elements[i]->getNumber() << " ";
    out << endl;
}

// ------------------------------------------------------------
void FemElementLoad::readFromStream(std::istream &in)
{
    int nElements;
    long elementIndex;
    FemLoad::readFromStream(in);
    in >> m_localDirection[0];
    in >> m_localDirection[1];
    in >> m_localDirection[2];
    in >> nElements;
    m_elementIndex.clear();
    for (int i=0; i<nElements; i++)
    {
        in >> elementIndex;
        m_elementIndex.push_back(elementIndex);
    }
}

// ------------------------------------------------------------
void FemElementLoad::setLocalDirection(double ex, double ey, double ez)
{
    m_localDirection[0] = ex;
    m_localDirection[1] = ey;
    m_localDirection[2] = ez;
}

// ------------------------------------------------------------
void FemElementLoad::getLocalDirection(double &ex, double &ey, double &ez)
{
    ex = m_localDirection[0];
    ey = m_localDirection[1];
    ez = m_localDirection[2];
}

// ------------------------------------------------------------
void FemElementLoad::setLocalDirection(double *v)
{
    m_localDirection[0] = v[0];
    m_localDirection[1] = v[1];
    m_localDirection[2] = v[2];
}

// ------------------------------------------------------------
void FemElementLoad::getLocalDirection(double *v)
{
    v[0] = m_localDirection[0];
    v[1] = m_localDirection[1];
    v[2] = m_localDirection[2];
}

// ------------------------------------------------------------
void FemElementLoad::addElement(FemElement *element)
{
    // We do not allow duplicates in the
    // element load.

    // This must be optimized!!

    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        FemElement* e = m_elements[i];
        if (e==element)
        {
            return;
        }
    }

    element->addReference();
    m_elements.push_back(element);
}

// ------------------------------------------------------------
void FemElementLoad::clearElements()
{
    for (unsigned int i=0; i<m_elements.size(); i++)
    {
        FemElement* e = m_elements[i];
        e->deleteReference();
    }
    m_elements.clear();
}

// ------------------------------------------------------------
bool FemElementLoad::removeElement(FemElement *element)
{
    std::vector<FemElement*>::iterator p = m_elements.begin();

    while ( (p!=m_elements.end())&&(*p!=element) )
        p++;

    if (*p==element)
    {
        element->deleteReference();
        m_elements.erase(p);
        return true;
    }
    return false;
}

// ------------------------------------------------------------
size_t FemElementLoad::getElementsSize()
{
    return m_elements.size();
}

// ------------------------------------------------------------
FemElement* FemElementLoad::getElement(unsigned int idx)
{
    if (idx<m_elements.size())
        return m_elements[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
size_t FemElementLoad::getElementIndexSize()
{
    return m_elementIndex.size();
}

// ------------------------------------------------------------
unsigned int FemElementLoad::getElementIndex(unsigned int idx)
{
    if (idx<m_elementIndex.size())
        return m_elementIndex[idx];
    else
        return -1;
}
