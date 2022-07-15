#pragma once

#include <ofem/load_set.h>

namespace ofem {

    SmartPointer(NodeLoadSet);

    class NodeLoadSet : public LoadSet
    {
    public:
        NodeLoadSet();
        virtual ~NodeLoadSet();

        // Class info

        ClassInfo("NodeLoadSet", LoadSet);

    protected:
        Load* createLoad();
    };
}
