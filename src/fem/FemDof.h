#ifndef _CFemDof_H_
#define _CFemDof_H_

#include "FemObject.h"

enum FemDofKind {
    DisplX,
    DisplY,
    DisplZ, 
    RotX,
    RotY,
    RotZ
};

FemSmartPointer(FemDof);

class FemDof : public FemObject {
private:
    FemDofKind m_kind;
    long m_number;
public:
    FemDof ();
    FemDof (FemDofKind kind, int number=-1);
    virtual ~FemDof ();

    // Class info

    FemClassInfo("FemDof",FemObject);

    // Methods

    virtual void print(std::ostream &out);
    virtual json toJSON();
    virtual void fromJSON(json& j);

    // Get/Set methods

    void setKind(FemDofKind kind);
    FemDofKind getKind();
    void setNumber(long number);
    long getNumber();
};
#endif
