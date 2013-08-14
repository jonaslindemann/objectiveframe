// Implementation of: public class CFemElementLoad

#include "FemElementLoad.h"

// ------------------------------------------------------------
CFemElementLoad::CFemElementLoad ()
		:CFemLoad()
{
	m_localDirection[0] = 0.0;
	m_localDirection[1] = 0.0;
	m_localDirection[2] = 1.0;
}

// ------------------------------------------------------------
CFemElementLoad::~CFemElementLoad ()
{
	this->clearElements();
}

// ------------------------------------------------------------
void CFemElementLoad::print(ostream &out)
{

}

// ------------------------------------------------------------
void CFemElementLoad::saveToStream(std::ostream &out)
{
	CFemLoad::saveToStream(out);
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
void CFemElementLoad::readFromStream(std::istream &in)
{
	int nElements;
	long elementIndex;
	CFemLoad::readFromStream(in);
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
void CFemElementLoad::setLocalDirection(double ex, double ey, double ez)
{
	m_localDirection[0] = ex;
	m_localDirection[1] = ey;
	m_localDirection[2] = ez;
}

// ------------------------------------------------------------
void CFemElementLoad::getLocalDirection(double &ex, double &ey, double &ez)
{
	ex = m_localDirection[0];
	ey = m_localDirection[1];
	ez = m_localDirection[2];
}

// ------------------------------------------------------------
void CFemElementLoad::setLocalDirection(double *v)
{
	m_localDirection[0] = v[0];
	m_localDirection[1] = v[1];
	m_localDirection[2] = v[2];
}

// ------------------------------------------------------------
void CFemElementLoad::getLocalDirection(double *v)
{
	v[0] = m_localDirection[0];
	v[1] = m_localDirection[1];
	v[2] = m_localDirection[2];
}

// ------------------------------------------------------------
void CFemElementLoad::addElement(CFemElement *element)
{
	// We do not allow duplicates in the
	// element load.

	// This must be optimized!!

	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		CFemElement* e = m_elements[i];
		if (e==element)
		{
			return;
		}
	}	

	element->addReference();
	m_elements.push_back(element);
}

// ------------------------------------------------------------
void CFemElementLoad::clearElements()
{
	for (unsigned int i=0; i<m_elements.size(); i++)
	{
		CFemElement* e = m_elements[i];
		e->deleteReference();
	}	
	m_elements.clear();
}

// ------------------------------------------------------------
bool CFemElementLoad::removeElement(CFemElement *element)
{
	std::vector<CFemElement*>::iterator p = m_elements.begin();

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
int CFemElementLoad::getElementsSize()
{
	return m_elements.size();
}

// ------------------------------------------------------------
CFemElement* CFemElementLoad::getElement(unsigned int idx)
{
	if ( (idx>=0)&&(idx<m_elements.size()) )
		return m_elements[idx];
	else
		return NULL;
}

// ------------------------------------------------------------
unsigned int CFemElementLoad::getElementIndexSize()
{
	return m_elementIndex.size();
}

// ------------------------------------------------------------
unsigned int CFemElementLoad::getElementIndex(unsigned int idx)
{
	if ( (idx>=0)&&(idx<m_elementIndex.size()) )
		return m_elementIndex[idx];
	else
		return -1;
}
