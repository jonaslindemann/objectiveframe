// Implementation of: public class CFemBC

#include <ofem/bc.h>

using namespace std;
using namespace ofem;


BC::BC()
    : Base()
    , m_number { -1 }
{
}


BC::~BC()
{
}


bool BC::isPrescribed(Dof* dof)
{
    std::vector<DofPtr>::iterator p = m_prescribedDofs.begin();
    std::vector<DofPtr>::iterator lastDof = m_prescribedDofs.end();

    while (p != lastDof)
    {
        if ((*p).get() == dof)
            return true;
        p++;
    }
    return false;
}


void BC::prescribeDof(Dof* dof, double value)
{
    if (!isPrescribed(dof))
    {
        m_prescribedDofs.push_back(DofPtr(dof));
        m_prescribedValues.push_back(value);
    }
}


void BC::unprescribeDof(Dof* dof)
{
    auto p = m_prescribedDofs.begin();
    auto q = m_prescribedValues.begin();
    auto lastDof = m_prescribedDofs.end();

    while (p != lastDof)
    {
        if ((*p).get() == dof)
        {
            m_prescribedDofs.erase(p);
            m_prescribedValues.erase(q);
        }
        p++;
        q++;
    }
}


void BC::clearDofs()
{
    m_prescribedDofs.clear();
    m_prescribedValues.clear();
}


void BC::setNumber(long number)
{
    m_number = number;
}


long BC::getNumber()
{
    return m_number;
}


void BC::saveToStream(std::ostream& out)
{
    Base::saveToStream(out);

    auto p = m_prescribedDofs.begin();
    auto q = m_prescribedValues.begin();
    auto lastDof = m_prescribedDofs.end();

    DofPtr dof;

    out << m_prescribedDofs.size() << endl;
    while (p != lastDof)
    {
        dof = *p;
        out << dof->getKind() << " ";
        out << dof->getNumber() << " ";
        out << *q << endl;
        p++;
        q++;
    }
}


json_nl BC::toJson()
{
    json_nl j;
    j["number"] = m_number;

    json_nl dofList;

    for (auto& pdof : m_prescribedDofs)
        dofList.push_back(pdof->toJson());

    j["prescribed_dofs"] = dofList;
    j["prescribed_values"] = m_prescribedValues;
    return j;
}


void BC::readFromStream(std::istream& in)
{
    Base::readFromStream(in);

    int nDofs, kind, number;
    double value;
    DofPtr dof;

    this->clearDofs();

    in >> nDofs;
    for (int i = 0; i < nDofs; i++)
    {
        dof = Dof::create();
        in >> kind;
        in >> number;
        in >> value;
        dof->setKind(static_cast<DofKind>(kind));
        dof->setNumber(number);
        m_prescribedDofs.push_back(dof);
        m_prescribedValues.push_back(value);
    }
}


void BC::print(std::ostream& out)
{
}


Dof* BC::getDof(unsigned int idx)
{
    if (idx < m_prescribedDofs.size())
        return m_prescribedDofs[idx].get();
    else
        return NULL;
}


double BC::getValue(unsigned int idx)
{
    if (idx < m_prescribedDofs.size())
        return m_prescribedValues[idx];
    else
        return 0.0;
}


size_t BC::getSize()
{
    return m_prescribedDofs.size();
}
