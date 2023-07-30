#pragma once

#include <ofem/base.h>
#include <ofem/dof.h>

#include <vector>

namespace ofem
{

enum NodeKind
{
    nk3Dof,
    nk6Dof,
    nkNotConnected
};

SmartPointer(Node);

class Node : public Base
{
private:
    long m_number;
    NodeKind m_kind;
    double m_coord[3];
    std::vector<double> m_values;
    std::vector<DofPtr> m_dofs;

public:
    Node();
    Node(double x, double y, double z);
    virtual ~Node();

    // Class info

    ClassInfo("Node", Base);
    StdFactory(Node);

    // Methods

    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;

    virtual json_nl toJson() override;
    virtual void fromJson(json_nl& j) override;

    long enumerateDofs(long count);

    void setValueSize(int size);
    double getValue(unsigned int idx);
    void setValue(unsigned int idx, double value);
    void clearValues();
    size_t getValueSize();

    // Get/Set methods

    void setCoord(double x, double y, double z);
    void getCoord(double& x, double& y, double& z);
    void setNumber(long number);
    long getNumber();
    NodeKind getKind();
    void setKind(NodeKind kind);
    Dof* getDof(unsigned int dof);

    // IO methods

    virtual void readFromStream(std::istream& in) override;
};
}
