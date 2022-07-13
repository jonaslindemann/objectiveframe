#ifndef _CFemLoad_H_
#define _CFemLoad_H_

#include "FemObject.h"

FemSmartPointer(FemLoad);

class FemLoad : public FemObject {
private:
    long m_number;
    double m_value;
public:
    FemLoad ();
    virtual ~FemLoad ();

    FemClassInfo("FemLoad",FemObject);

    // Get/set methods

    long getNumber();
    void setNumber(long number);
    double getValue();
    void setValue(double value);

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
