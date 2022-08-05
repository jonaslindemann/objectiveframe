#pragma once

#include <ofem/bc.h>
#include <ofem/node.h>

namespace ofem
{

SmartPointer(NodeBC);

class NodeBC : public BC
{
private:
    std::vector<Node*> m_nodes;
    std::vector<long> m_nodeIndex;
    bool m_prescribedDof[6];
    double m_prescribedValues[6];

public:
    NodeBC();
    virtual ~NodeBC();

    // Class info

    ClassInfo("NodeBC", BC);

    // Methods

    bool removeNode(Node* node);
    void clearNodes();
    void addNode(Node* node);
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
    Node* getNode(unsigned int idx);
    size_t getNodeSize();
    size_t getNodeIndexSize();
    double getPrescribedValue(int dof);

    bool* getPrescribedArr();
    double* getPrescribedValueArr();

    // IO Methods

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;

    virtual json_nl toJson() override;
};
}
