#include "FemInternalDofs.h"

// ------------------------------------------------------------
FemInternalDofs::FemInternalDofs()
{
    // Default we define 6 internal dofs.

    this->setKind(FEM_DISPL_ROT_DOFS);
}

// ------------------------------------------------------------
FemInternalDofs::~FemInternalDofs()
{
    // Delete any extra dofs, if specified

    unsigned int i;
    for (i=0; i<m_dofs.size(); i++)
        if (m_dofs[i]!=NULL)
            delete m_dofs[i];
}

// ------------------------------------------------------------
void FemInternalDofs::setKind(int kind)
{
    m_kind = kind;
    unsigned int i;

    // Delete any defined dofs in list
    // if any.

    for (i=0; i<m_dofs.size(); i++)
        if (m_dofs[i]!=NULL)
            delete m_dofs[i];

    m_dofs.clear();

    // Create an empty list of dofs

    switch (m_kind) {
    case FEM_DISPL_ROT_DOFS:
        for (i=0; i<6; i++)
            m_dofs.push_back(NULL);
        break;
    case FEM_DISPL_DOFS:
        for (i=0; i<3; i++)
            m_dofs.push_back(NULL);
        break;
    default:
        for (i=0; i<6; i++)
            m_dofs.push_back(NULL);
        break;
    };
}

// ------------------------------------------------------------
int FemInternalDofs::getKind()
{
    return m_kind;
}

// ------------------------------------------------------------
void FemInternalDofs::add(unsigned int localDof)
{
    // Add an extra dof to a specified local dof

    if (localDof<m_dofs.size())
    {
        FemDof* dof = new FemDof();
        if (m_dofs[localDof]!=NULL)
            delete m_dofs[localDof];
        m_dofs[localDof] = dof;
    }
}

// ------------------------------------------------------------
void FemInternalDofs::clear(unsigned int localDof)
{
    // Clear extra dof at local dof

    if (localDof<m_dofs.size())
    {
        if (m_dofs[localDof]!=NULL)
            delete m_dofs[localDof];
        m_dofs[localDof] = NULL;
    }
}

// ------------------------------------------------------------
long FemInternalDofs::enumerateDofs(long count)
{
    // Enumerate dofs

    for (long i=0; i<(long)m_dofs.size(); i++)
    {
        if (m_dofs[i]!=NULL)
            m_dofs[i]->setNumber(count++);
    }
    return count;

}

// ------------------------------------------------------------
bool FemInternalDofs::isAssigned(unsigned int localDof)
{
    if (localDof<m_dofs.size())
    {
        if (m_dofs[localDof]!=NULL)
            return true;
    }
    return false;
}

// ------------------------------------------------------------
FemDof* FemInternalDofs::getDof(unsigned int localDof)
{
    if (localDof<m_dofs.size())
    {
        if (m_dofs[localDof]!=NULL)
            return m_dofs[localDof];
    }
    return NULL;
}

// ------------------------------------------------------------
void FemInternalDofs::readFromStream(std::istream &in)
{
    unsigned int i;
    int kind;
    int idof;
    FemObject::readFromStream(in);
    in >> kind;
    this->setKind(kind);
    for (i=0; i<m_dofs.size(); i++)
    {
        in >> idof;
        if (idof!=0)
        {
            FemDof* dof = new FemDof();
            m_dofs[i] = dof;
        }
    }
}

// ------------------------------------------------------------
void FemInternalDofs::saveToStream(std::ostream &out)
{
    using namespace std;
    unsigned int i;
    FemObject::saveToStream(out);
    out << m_kind << endl;
    for (i=0; i<m_dofs.size(); i++)
    {
        if (m_dofs[i]==NULL)
            out << 0 << " ";
        else
            out << 1 << " ";
    }
    out << endl;
}
