#ifndef _CFemNodeBC_H_
#define _CFemNodeBC_H_

#include "FemBC.h"
#include "FemNode.h"

FemSmartPointer(FemNodeBC);

class FemNodeBC : public FemBC {
private:
    std::vector<FemNode*> m_nodes;
    std::vector<long> m_nodeIndex;
    bool m_prescribedDof[6];
    double m_prescribedValues[6];
public:
    FemNodeBC ();
    virtual ~FemNodeBC ();

    // Class info

    FemClassInfo("FemNodeBC",FemBC);

    // Methods

    bool removeNode(FemNode* node);
    void clearNodes();
    void addNode(FemNode* node);
    bool isPrescribed(int dof);
    void unprescribe(int dof);
    void prescribe(int dof, double value);
	void prescribePos(double value);
	void prescribeRot(double value);

	void fixed();
	void fixedPosition();
	void release();

    // Get/set methods

    long getNodeIndex(unsigned int idx);
    FemNode* getNode(unsigned int idx);
    size_t getNodeSize();
    size_t getNodeIndexSize();
    double getPrescribedValue(int dof);

    bool* getPrescribedArr();
    double* getPrescribedValueArr();

    // IO Methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;

    virtual json toJSON() override;
};
#endif
