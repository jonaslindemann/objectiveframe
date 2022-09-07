#include <ofem/node_load.h>
#include <ofem/node_load_set.h>

using namespace ofem;


NodeLoadSet::NodeLoadSet()
    : LoadSet()
{
}


NodeLoadSet::~NodeLoadSet()
{
}


Load* NodeLoadSet::createLoad()
{
    return new NodeLoad();
}
