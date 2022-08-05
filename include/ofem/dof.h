#pragma once

#include <ofem/base.h>

namespace ofem
{

enum DofKind
{
    DisplX,
    DisplY,
    DisplZ,
    RotX,
    RotY,
    RotZ
};

SmartPointer(Dof);

class Dof : public Base
{
private:
    DofKind m_kind;
    long m_number;

public:
    Dof();
    Dof(DofKind kind, int number = -1);
    virtual ~Dof();

    // Class info

    ClassInfo("Dof", Base);

    // Methods

    virtual void print(std::ostream& out) override;
    virtual json_nl toJson() override;
    virtual void fromJson(json_nl& j) override;

    // Get/Set methods

    void setKind(DofKind kind);
    DofKind getKind();
    void setNumber(long number);
    long getNumber();
};
}
