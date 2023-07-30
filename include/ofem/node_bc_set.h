#pragma once

#include <ofem/bc.h>
#include <ofem/bc_set.h>

namespace ofem
{

SmartPointer(NodeBCSet);

class NodeBCSet : public BCSet
{
public:
    NodeBCSet();
    virtual ~NodeBCSet();

    // Class info

    ClassInfo("NodeBCSet", BCSet);
    StdFactory(NodeBCSet);

protected:
    virtual BC* createBC() override;
};
}
