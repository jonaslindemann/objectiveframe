#include <ofem/node_bc.h>
#include <ofem/node_bc_set.h>

using namespace ofem;


NodeBCSet::NodeBCSet()
    : BCSet()
{
}


NodeBCSet::~NodeBCSet()
{
}


BC* NodeBCSet::createBC()
{
    return (BC*)new NodeBC();
}
