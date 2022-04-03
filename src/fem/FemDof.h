#ifndef _CFemDof_H_
#define _CFemDof_H_

#include "FemObject.h"

#define FEM_DISPL_X  0
#define FEM_DISPL_Y  1
#define FEM_DISPL_Z  2
#define FEM_ROT_X	 3
#define FEM_ROT_Y	 4
#define FEM_ROT_Z	 5

FemSmartPointer(CFemDof);

class CFemDof : public CFemObject {
private:
    int m_kind;
    long m_number;
public:
    CFemDof ();
    CFemDof (int kind, int number=-1);
    virtual ~CFemDof ();

    // Class info

    FemClassInfo("CFemDof",CFemObject);

    // Methods

    virtual void print(std::ostream &out);
    virtual json toJSON();
    virtual void fromJSON(json& j);

    // Get/Set methods

    void setKind(int kind);
    int getKind();
    void setNumber(long number);
    long getNumber();
};
#endif
