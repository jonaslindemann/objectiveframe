// Implementation of: public class CFemBC

#include "FemBC.h"

using namespace std;

// ------------------------------------------------------------
FemBC::FemBC ()
    :FemObject(), m_number{-1}
{
}

// ------------------------------------------------------------
FemBC::~FemBC ()
{
}

// ------------------------------------------------------------
bool FemBC::isPrescribed(FemDof* dof)
{
    std::vector<FemDofPtr>::iterator p = m_prescribedDofs.begin();
    std::vector<FemDofPtr>::iterator lastDof = m_prescribedDofs.end();

    while (p!=lastDof)
    {
        if (*p==dof)
            return true;
        p++;
    }
    return false;
}

// ------------------------------------------------------------
void FemBC::prescribeDof(FemDof* dof, double value)
{
    if (!isPrescribed(dof))
    {
        m_prescribedDofs.push_back(FemDofPtr(dof));
        m_prescribedValues.push_back(value);
    }
}

// ------------------------------------------------------------
void FemBC::unprescribeDof(FemDof* dof)
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
void FemBC::clearDofs()
{
    m_prescribedDofs.clear();
    m_prescribedValues.clear();
}

// ------------------------------------------------------------
void FemBC::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemBC::getNumber()
{
    return m_number;
}

// ------------------------------------------------------------
void FemBC::saveToStream(std::ostream &out)
{
    FemObject::saveToStream(out);

    auto p = m_prescribedDofs.begin();
    auto q = m_prescribedValues.begin();
    auto lastDof = m_prescribedDofs.end();

    FemDof* dof;

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
json FemBC::toJSON()
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
void FemBC::readFromStream(std::istream &in)
{
    FemObject::readFromStream(in);

    int nDofs, kind, number;
    double value;
    FemDofPtr dof;

    this->clearDofs();

    in >> nDofs;
    for (int i=0; i<nDofs; i++)
    {
        dof = new FemDof();
        in >> kind;
        in >> number;
        in >> value;
        dof->setKind(static_cast<FemDofKind>(kind));
        dof->setNumber(number);
        m_prescribedDofs.push_back(FemDofPtr(dof));
        m_prescribedValues.push_back(value);
    }
}

// ------------------------------------------------------------
void FemBC::print(std::ostream &out)
{

}


// ------------------------------------------------------------
FemDof* FemBC::getDof(unsigned int idx)
{
    if (idx<m_prescribedDofs.size())
        return m_prescribedDofs[idx];
    else
        return NULL;
}

// ------------------------------------------------------------
double FemBC::getValue(unsigned int idx)
{
    if (idx<m_prescribedDofs.size())
        return m_prescribedValues[idx];
    else
        return 0.0;
}

// ------------------------------------------------------------
size_t FemBC::getSize()
{
    return m_prescribedDofs.size();
}
