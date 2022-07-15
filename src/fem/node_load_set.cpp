#include <ofem/node_load_set.h>
#include <ofem/node_load.h>

using namespace ofem;

// ------------------------------------------------------------
NodeLoadSet::NodeLoadSet()
    :LoadSet()
{

}

// ------------------------------------------------------------
NodeLoadSet::~NodeLoadSet()
{

}

// ------------------------------------------------------------
Load* NodeLoadSet::createLoad()
{
    return new NodeLoad();
}
