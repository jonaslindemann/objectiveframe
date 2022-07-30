#include <ofem/element_load.h>
#include <ofem/element_load_set.h>

using namespace ofem;

ElementLoadSet::ElementLoadSet()
    : LoadSet()
{
}

ElementLoadSet::~ElementLoadSet()
{
}

Load* ElementLoadSet::createLoad()
{
    return new ElementLoad();
}
