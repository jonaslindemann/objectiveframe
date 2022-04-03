#ifndef _CFemNodeBC_H_
#define _CFemNodeBC_H_

#include "FemBC.h"
#include "FemNode.h"

FemSmartPointer(CFemNodeBC);

class CFemNodeBC : public CFemBC {
private:
    std::vector<CFemNode*> m_nodes;
    std::vector<long> m_nodeIndex;
    bool m_prescribedDof[6];
    double m_prescribedValues[6];
public:
    CFemNodeBC ();
    virtual ~CFemNodeBC ();

    // Class info

    FemClassInfo("CFemNodeBC",CFemBC);

    // Methods

    bool removeNode(CFemNode* node);
    void clearNodes();
    void addNode(CFemNode* node);
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
    CFemNode* getNode(unsigned int idx);
    int getNodeSize();
    int getNodeIndexSize();
    double getPrescribedValue(int dof);

    // IO Methods

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
    virtual void print(std::ostream &out) override;

    virtual json toJSON() override;
};
#endif
