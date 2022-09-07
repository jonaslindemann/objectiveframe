// Implementation of: public class CFemObject

#include <ofem/base.h>

#include <iostream>

using namespace std;
using namespace ofem;

Base::Base()
{
    m_ref = 0;
    m_user = NULL;
    m_readOnly = false;
}

Base::~Base()
{
}

void Base::addReference()
{
    m_ref++;
    // cout << this->getClassName() << "->addReference() (refs = " << m_ref << ")" << endl;
}

void Base::deleteReference()
{
    if (m_ref > 0)
        m_ref--;
    // cout << this->getClassName() << "->deleteReference() (refs = " << m_ref << ")" << endl;
}

bool Base::isReferenced()
{
    return (m_ref > 0);
}

const std::string Base::getClassNameThis()
{
    return "FemObject";
}

const std::string Base::getClassName()
{
    return "FemObject";
}

bool Base::isClass(const std::string& name)
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

void Base::saveToStream(std::ostream& out)
{
    // out << getClassId() << endl;
}

void Base::readFromStream(std::istream& in)
{
    // int classId;
    // in >> classId;
}

void Base::print(std::ostream& out)
{
}

json_nl Base::toJson()
{
    nlohmann::json j;
    return j;
}

void Base::fromJson(json_nl& j)
{
}

void Base::setUser(void* user)
{
    m_user = user;
}

void* Base::getUser()
{
    return m_user;
}

int Base::getRefCount()
{
    return m_ref;
}

bool Base::isReadOnly()
{
    return m_readOnly;
}

void Base::setReadOnly(bool flag)
{
    m_readOnly = flag;
}
