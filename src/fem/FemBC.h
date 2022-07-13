#ifndef _CFemBC_H_
#define _CFemBC_H_

#include "FemObject.h"
#include "FemDof.h"

#include <vector>

FemSmartPointer(FemBC);

class FemBC : public FemObject {
private:
    long m_number;
    std::vector<FemDofPtr> m_prescribedDofs;
    std::vector<double>   m_prescribedValues;
public:
    FemBC ();
    virtual ~FemBC ();

    FemClassInfo("FemBC",FemObject);

    // Methods

    bool isPrescribed(FemDof* dof);
    void prescribeDof(FemDof* dof, double value);
    void unprescribeDof(FemDof* dof);
    void clearDofs();

    // Get/set methods

    void setNumber(long number);
    long getNumber();

    size_t getSize();

    double getValue(unsigned int idx);

    FemDof* getDof(unsigned int idx);

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);

    virtual json toJSON();
};
#endif
