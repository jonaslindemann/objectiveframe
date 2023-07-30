#pragma once

#include <ofem/load.h>
#include <ofem/node.h>

#include <vector>

namespace ofem
{

SmartPointer(NodeLoad);

class NodeLoad : public Load
{
private:
    std::vector<Node*> m_nodes;
    std::vector<long> m_nodeIndex;
    double m_direction[3];

public:
    NodeLoad();
    virtual ~NodeLoad();

    // Class info

    ClassInfo("NodeLoad", Load);
    StdFactory(NodeLoad);

    // Methods

    bool removeNode(Node* node);
    void clearNodes();
    void addNode(Node* node);

    // Get/set methods

    void getDirection(double* v);
    void setDirection(double* v);
    void getDirection(double& ex, double& ey, double& ez);
    void setDirection(double ex, double ey, double ez);
    long getNodeIndex(unsigned int idx);
    Node* getNode(unsigned int idx);
    size_t getNodeSize();
    size_t getNodeIndexSize();

    // IO Methods

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;
};
}
