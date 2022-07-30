#pragma once

#include <ofem/base.h>
#include <ofem/dof.h>

#include <vector>

#define FEM_DISPL_NODE 0
#define FEM_DISPL_ROT_NODE 1

namespace ofem
{

SmartPointer(Node);

class Node : public Base
{
private:
    long m_number;
    int m_kind;
    double m_coord[3];
    std::vector<double> m_values;
    std::vector<DofPtr> m_dofs;

public:
    Node();
    Node(double x, double y, double z);
    virtual ~Node();

    // Class info

    ClassInfo("Node", Base);

    // Methods

    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;

    virtual json toJSON() override;
    virtual void fromJSON(json& j) override;

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
    int getKind();
    void setKind(int kind);
    Dof* getDof(unsigned int dof);

    // IO methods

    virtual void readFromStream(std::istream& in) override;
};
}
