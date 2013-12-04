#ifndef _CFemInternalDofs_h_
#define _CFemInternalDofs_h_

#include "FemObject.h"

#include "FemDof.h"

#include <vector>

#define FEM_DISPL_DOFS      0
#define FEM_DISPL_ROT_DOFS  1

FemSmartPointer(CFemInternalDofs);

class CFemInternalDofs : public CFemObject  {
private:
    std::vector<CFemDof*> m_dofs;
    int m_kind;
public:
    CFemInternalDofs();
    virtual ~CFemInternalDofs();

    FemClassInfo("CFemInternalDofs",CFemObject);

    // Get/set methods

    int getKind();
    void setKind(int kind);
    CFemDof* getDof(unsigned int localDof);

    // Methods

    long enumerateDofs(long count);
    void clear(unsigned int localDof);
    void add(unsigned int localDof);
    bool isAssigned(unsigned int localDof);

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);

};

#endif
