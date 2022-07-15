#pragma once

#include <ofem/bc_set.h>
#include <ofem/bc.h>

namespace ofem {

    SmartPointer(NodeBCSet);

    class NodeBCSet : public BCSet
    {
    public:
        NodeBCSet();
        virtual ~NodeBCSet();

        // Class info

        ClassInfo("NodeBCSet", BCSet);
    protected:
        virtual BC* createBC();
    };
}
