// Implementation of: public class CFemBC

#include "FemBC.h"

using namespace std;

// ------------------------------------------------------------
CFemBC::CFemBC ()
    :CFemObject(), m_number{-1}
{
}

// ------------------------------------------------------------
CFemBC::~CFemBC ()
{
}

// ------------------------------------------------------------
bool CFemBC::isPrescribed(CFemDof* dof)
{
    std::vector<CFemDofPtr>::iterator p = m_prescribedDofs.begin();
    std::vector<CFemDofPtr>::iterator lastDof = m_prescribedDofs.end();

    while (p!=lastDof)
    {
        if (*p==dof)
            return true;
        p++;
    }
    return false;
}

// ------------------------------------------------------------
void CFemBC::prescribeDof(CFemDof* dof, double value)
{
    if (!isPrescribed(dof))
    {
        m_prescribedDofs.push_back(CFemDofPtr(dof));
        m_prescribedValues.push_back(value);
    }
}

// ------------------------------------------------------------
void CFemBC::unprescribeDof(CFemDof* dof)
{
    auto p = m_prescribedDofs.begin();
    auto q = m_prescribedValues.begin();
    auto lastDof = m_prescribedDofs.end();

    while (p!=lastDof)
    {
        if (*p==dof)
        {
            m_prescribedDofs.erase(p);
            m_prescribedValues.erase(q);
        }
        p++;
        q++;
    }
}

// ------------------------------------------------------------
void CFemBC::clearDofs()
{
    m_prescribedDofs.clear();
    m_prescribedValues.clear();
}

// ------------------------------------------------------------
void CFemBC::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long CFemBC::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void CFemBC::saveToStream(std::ostream &out)
{
    CFemObject::saveToStream(out);

    auto p = m_prescribedDofs.begin();
    auto q = m_prescribedValues.begin();
    auto lastDof = m_prescribedDofs.end();

    CFemDof* dof;

    out << m_prescribedDofs.size() << endl;
    while (p!=lastDof)
    {
        dof = *p;
        out << dof->getKind() << " ";
        out << dof->getNumber() << " ";
        out << *q << endl;
        p++;
        q++;
    }
}

// ------------------------------------------------------------
json CFemBC::toJSON()
{
    json j;
    j["number"] = m_number;

    json dofList;

    for (auto& pdof : m_prescribedDofs)
        dofList.push_back(pdof->toJSON());

    j["prescribed_dofs"] = dofList;
    j["prescribed_values"] = m_prescribedValues;
    return j;
}

// ------------------------------------------------------------
void CFemBC::readFromStream(std::istream &in)
{
    CFemObject::readFromStream(in);

    int nDofs, kind, number;
    double value;
    CFemDofPtr dof;

    this->clearDofs();

    in >> nDofs;
    for (int i=0; i<nDofs; i++)
    {
        dof = new CFemDof();
        in >> kind;
        in >> number;
        in >> value;
        dof->setKind(kind);
        dof->setNumber(number);
        m_prescribedDofs.push_back(CFemDofPtr(dof));
        m_prescribedValues.push_back(value);
    }
}

// ------------------------------------------------------------
void CFemBC::print(std::ostream &out)
{

}


// ------------------------------------------------------------
CFemDof* CFemBC::getDof(unsigned int idx)
{
    if (idx<m_prescribedDofs.size())
        return m_prescribedDofs[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
double CFemBC::getValue(unsigned int idx)
{
    if (idx<m_prescribedDofs.size())
        return m_prescribedValues[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
size_t CFemBC::getSize()
{
    return m_prescribedDofs.size();
}
