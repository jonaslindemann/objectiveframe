#ifndef _CFemMaterial_H_
#define _CFemMaterial_H_

#include "FemObject.h"

FemSmartPointer(FemMaterial);

class FemMaterial : public FemObject {
private:
    long m_number;
public:
    FemMaterial ();
    virtual ~FemMaterial ();

    FemClassInfo("FemMaterial",FemObject);

    // Get/set methods

    long getNumber();
    void setNumber(long number);

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
