#include <ofem/dof.h>

using namespace ofem;

Dof::Dof()
    : Base()
{
    m_kind = DisplX;
    m_number = -1;
}

Dof::Dof(DofKind kind, int number)
    : Base()
{
    m_kind = kind;
    m_number = number;
}

Dof::~Dof()
{
}

DofPtr ofem::Dof::create(DofKind kind, int number)
{
    return DofPtr(new Dof(kind, number));
}

void Dof::print(std::ostream& out)
{
    out << m_kind + 1 << " ";
}

json_nl Dof::toJson()
{
    json_nl j;
    j["kind"] = m_kind;
    j["number"] = m_number;
    return j;
}

void Dof::fromJson(json_nl& j)
{
    // m_kind = j.at("kind").get<int>();
    // m_number = j.at("number").get<long>();
}

void Dof::setKind(DofKind kind)
{
    m_kind = kind;
}

DofKind Dof::getKind()
{
    return m_kind;
}

void Dof::setNumber(long number)
{
    m_number = number;
}

long Dof::getNumber()
{
    return m_number;
}
