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

        // Methods

        void deleteAll();
        Node* removeNode(long i);
        bool removeNode(Node* node);
        bool deleteNode(long i);
        Node* getNode(long i);
        void addNode(Node* node);
        void clear();

        void clearNodeValues();

        long enumerateNodes(long count = 1);
        long enumerateDofs(long count = 1);

        // Get/set methods

        size_t getSize();

        virtual void print(std::ostream& out);
        virtual void readFromStream(std::istream& in);
        virtual void saveToStream(std::ostream& out);

        virtual json toJSON() override;
    };
}
