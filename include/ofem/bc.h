#pragma once

#include <ofem/base.h>
#include <ofem/dof.h>

#include <vector>

namespace ofem {

SmartPointer(BC);

class BC : public Base {
private:
    long m_number;
    std::vector<DofPtr> m_prescribedDofs;
    std::vector<double> m_prescribedValues;

public:
    BC();
    virtual ~BC();

    ClassInfo("BC", Base);

    StdFactory(BC);

    // Methods

    bool isPrescribed(Dof *dof);
    void prescribeDof(Dof *dof, double value);
    void unprescribeDof(Dof *dof);
    void clearDofs();

    // Get/set methods

    void setNumber(long number);
    long getNumber();

    size_t getSize();

    double getValue(unsigned int idx);

    Dof *getDof(unsigned int idx);

    // IO Methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;

    virtual json_nl toJson() override;
};
} // namespace ofem
