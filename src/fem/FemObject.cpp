// Implementation of: public class CFemObject

#include "FemObject.h"

// ------------------------------------------------------------
CFemObject::CFemObject ()
{
	m_ref = 0;
	m_user = NULL;
}

// ------------------------------------------------------------
CFemObject::~CFemObject ()
{
}

// ------------------------------------------------------------
void CFemObject::addReference()
{
	m_ref++;
}

// ------------------------------------------------------------
void CFemObject::deleteReference()
{
	if (m_ref>0)
		m_ref--;
}

// ------------------------------------------------------------
bool CFemObject::isReferenced()
{
	return (m_ref>0);
}

// ------------------------------------------------------------
const std::string CFemObject::getClassNameThis()
{
	return "CFemObject";
}

// ------------------------------------------------------------
const std::string CFemObject::getClassName()
{
	return "CFemObject";
}

// ------------------------------------------------------------
bool CFemObject::isClass(const std::string& name)
{
	std::string className = this->getClassNameThis();

	if (!className.empty())
	{
		if (className == name)
			return true;
		else
			return false;
	}
	else
		return false;
}

// ------------------------------------------------------------
void CFemObject::saveToStream(std::ostream &out)
{
	//out << getClassId() << endl;
}

// ------------------------------------------------------------
void CFemObject::readFromStream(std::istream &in)
{
	//int classId;
	//in >> classId;
}

// ------------------------------------------------------------
void CFemObject::print(std::ostream &out)
{

}

// ------------------------------------------------------------
void CFemObject::setUser(void *user)
{
	m_user = user;
}

// ------------------------------------------------------------
void* CFemObject::getUser()
{
	return m_user;
}

// ------------------------------------------------------------
int CFemObject::getRefCount()
{
	return m_ref;
}
