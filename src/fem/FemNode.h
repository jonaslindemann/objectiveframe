#ifndef _CFemNode_H_
#define _CFemNode_H_

#include "FemObject.h"
#include "FemDof.h"

#include <vector>

#define FEM_DISPL_NODE      0
#define FEM_DISPL_ROT_NODE  1

FemSmartPointer(FemNode);

class FemNode : public FemObject {
private:
    long m_number;
    int m_kind;
    double m_coord[3];
    std::vector<double> m_values;
    std::vector<FemDofPtr> m_dofs;
public:
    FemNode ();
    FemNode (double x, double y, double z);
    virtual ~FemNode ();

    // Class info

    FemClassInfo("FemNode",FemObject);

    // Methods

    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);

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
    void getCoord(double &x, double &y, double &z);
    void setNumber(long number);
    long getNumber();
    int getKind();
    void setKind(int kind);
    FemDof* getDof(unsigned int dof);

    // IO methods

    virtual void readFromStream(std::istream &in);
};
#endif
