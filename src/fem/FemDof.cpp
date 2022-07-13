// Implementation of: public class CFemDof

#include "FemDof.h"

// ------------------------------------------------------------
FemDof::FemDof ()
    :FemObject()
{
    m_kind = DisplX;
    m_number = -1;
}

// ------------------------------------------------------------
FemDof::FemDof (FemDofKind kind, int number)
    :FemObject()
{
    m_kind = kind;
    m_number = number;
}

// ------------------------------------------------------------
FemDof::~FemDof ()
{
}

// ------------------------------------------------------------
void FemDof::print(std::ostream &out)
{
    out << m_kind+1 << " ";
}

json FemDof::toJSON()
{
    json j;
    j["kind"] = m_kind;
    j["number"] = m_number;
    return j;
}

void FemDof::fromJSON(json& j)
{
    //m_kind = j.at("kind").get<int>();
    //m_number = j.at("number").get<long>();
}

// ------------------------------------------------------------
void FemDof::setKind(FemDofKind kind)
{
    m_kind = kind;
}

// ------------------------------------------------------------
FemDofKind FemDof::getKind()
{
    return m_kind;
}

// ------------------------------------------------------------
void FemDof::setNumber(long number)
{
    m_number = number;
}

// ------------------------------------------------------------
long FemDof::getNumber()
{
    return m_number;
}
