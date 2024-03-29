#pragma once

#include <ofem/base.h>
#include <ofem/node.h>

#include <vector>

namespace ofem {

SmartPointer(NodeSet);

class NodeSet : public Base {
private:
    std::vector<NodePtr> m_nodes;

public:
    NodeSet();
    virtual ~NodeSet();

    // Class info

    ClassInfo("NodeSet", Base);
    StdFactory(NodeSet);

    // Methods

    void deleteAll();
    bool removeNode(long i);
    bool removeNode(Node *node);
    bool deleteNode(long i);
    Node *getNode(long i);
    void addNode(Node *node);
    void clear();

    void clearNodeValues();

    long enumerateNodes(long count = 1);
    long enumerateDofs(long count = 1);

    void resetNodeKind(NodeKind newKind);

    // Get/set methods

    size_t getSize();

    virtual void print(std::ostream &out) override;
    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;

    virtual json_nl toJson() override;
};
} // namespace ofem
