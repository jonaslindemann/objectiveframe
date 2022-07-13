// Implementation of: public class CFemObject

#include "FemObject.h"

#include <iostream>

using namespace std;

// ------------------------------------------------------------
FemObject::FemObject ()
{
    m_ref = 0;
    m_user = NULL;
	m_readOnly = false;
}

// ------------------------------------------------------------
FemObject::~FemObject ()
{
}

// ------------------------------------------------------------
void FemObject::addReference()
{
    m_ref++;
	//cout << this->getClassName() << "->addReference() (refs = " << m_ref << ")" << endl;
}

// ------------------------------------------------------------
void FemObject::deleteReference()
{
	if (m_ref>0)
        m_ref--;
	//cout << this->getClassName() << "->deleteReference() (refs = " << m_ref << ")" << endl;
}

// ------------------------------------------------------------
bool FemObject::isReferenced()
{
    return (m_ref>0);
}

// ------------------------------------------------------------
const std::string FemObject::getClassNameThis()
{
    return "FemObject";
}

// ------------------------------------------------------------
const std::string FemObject::getClassName()
{
    return "FemObject";
}

// ------------------------------------------------------------
bool FemObject::isClass(const std::string& name)
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
void FemObject::saveToStream(std::ostream &out)
{
    //out << getClassId() << endl;
}

// ------------------------------------------------------------
void FemObject::readFromStream(std::istream &in)
{
    //int classId;
    //in >> classId;
}

// ------------------------------------------------------------
void FemObject::print(std::ostream &out)
{

}

json FemObject::toJSON()
{
    nlohmann::json j;
    return j;
}

void FemObject::fromJSON(json& j)
{
}

// ------------------------------------------------------------
void FemObject::setUser(void *user)
{
    m_user = user;
}

// ------------------------------------------------------------
void* FemObject::getUser()
{
    return m_user;
}

// ------------------------------------------------------------
int FemObject::getRefCount()
{
    return m_ref;
}

bool FemObject::isReadOnly()
{
	return m_readOnly;
}

void FemObject::setReadOnly(bool flag)
{
	m_readOnly = flag;
}

