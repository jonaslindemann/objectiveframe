#pragma once

#include <ofem/base.h>
#include <ofem/dof.h>

#include <vector>

#define FEM_DISPL_DOFS 0
#define FEM_DISPL_ROT_DOFS 1

namespace ofem {

SmartPointer(InternalDofs);

class InternalDofs : public Base {
private:
    std::vector<Dof *> m_dofs;
    int m_kind;

public:
    InternalDofs();
    virtual ~InternalDofs();

    ClassInfo("InternalDofs", Base);
    StdFactory(InternalDofs);

    // Get/set methods

    int getKind();
    void setKind(int kind);
    Dof *getDof(unsigned int localDof);

    // Methods

    long enumerateDofs(long count);
    void clear(unsigned int localDof);
    void add(unsigned int localDof);
    bool isAssigned(unsigned int localDof);

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
};
} // namespace ofem
