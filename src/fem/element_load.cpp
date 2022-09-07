#include <ofem/element_load.h>

using namespace ofem;
using namespace std;


ElementLoad::ElementLoad()
    : Load()
{
    m_localDirection[0] = 0.0;
    m_localDirection[1] = 0.0;
    m_localDirection[2] = 1.0;
}


ElementLoad::~ElementLoad()
{
    this->clearElements();
}


void ElementLoad::print(ostream& out)
{
}


void ElementLoad::saveToStream(std::ostream& out)
{
    Load::saveToStream(out);
    out << m_localDirection[0] << " ";
    out << m_localDirection[1] << " ";
    out << m_localDirection[2] << " ";
    out << endl;
    out << m_elements.size() << " ";
    for (unsigned int i = 0; i < m_elements.size(); i++)
        out << m_elements[i]->getNumber() << " ";
    out << endl;
}


void ElementLoad::readFromStream(std::istream& in)
{
    int nElements;
    long elementIndex;
    Load::readFromStream(in);
    in >> m_localDirection[0];
    in >> m_localDirection[1];
    in >> m_localDirection[2];
    in >> nElements;
    m_elementIndex.clear();
    for (int i = 0; i < nElements; i++)
    {
        in >> elementIndex;
        m_elementIndex.push_back(elementIndex);
    }
}


void ElementLoad::setLocalDirection(double ex, double ey, double ez)
{
    m_localDirection[0] = ex;
    m_localDirection[1] = ey;
    m_localDirection[2] = ez;
}


void ElementLoad::getLocalDirection(double& ex, double& ey, double& ez)
{
    ex = m_localDirection[0];
    ey = m_localDirection[1];
    ez = m_localDirection[2];
}


void ElementLoad::setLocalDirection(double* v)
{
    m_localDirection[0] = v[0];
    m_localDirection[1] = v[1];
    m_localDirection[2] = v[2];
}


void ElementLoad::getLocalDirection(double* v)
{
    v[0] = m_localDirection[0];
    v[1] = m_localDirection[1];
    v[2] = m_localDirection[2];
}


void ElementLoad::addElement(Element* element)
{
    // We do not allow duplicates in the
    // element load.

    // This must be optimized!!

    for (unsigned int i = 0; i < m_elements.size(); i++)
    {
        Element* e = m_elements[i];
        if (e == element)
        {
            return;
        }
    }

    element->addReference();
    m_elements.push_back(element);
}


void ElementLoad::clearElements()
{
    for (unsigned int i = 0; i < m_elements.size(); i++)
    {
        Element* e = m_elements[i];
        e->deleteReference();
    }
    m_elements.clear();
}


bool ElementLoad::removeElement(Element* element)
{
    std::vector<Element*>::iterator p = m_elements.begin();

    while ((p != m_elements.end()) && (*p != element))
        p++;

    if (*p == element)
    {
        element->deleteReference();
        m_elements.erase(p);
        return true;
    }
    return false;
}


size_t ElementLoad::getElementsSize()
{
    return m_elements.size();
}


Element* ElementLoad::getElement(unsigned int idx)
{
    if (idx < m_elements.size())
        return m_elements[idx];
    else
        return NULL;
}


size_t ElementLoad::getElementIndexSize()
{
    return m_elementIndex.size();
}


unsigned int ElementLoad::getElementIndex(unsigned int idx)
{
    if (idx < m_elementIndex.size())
        return m_elementIndex[idx];
    else
        return -1;
}
