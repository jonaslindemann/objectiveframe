#include <ofem/beam_load.h>
#include <ofem/beam_load_set.h>

using namespace ofem;


BeamLoadSet::BeamLoadSet()
    : ElementLoadSet()
{
}


BeamLoadSet::~BeamLoadSet()
{
}


Load* BeamLoadSet::createLoad()
{
    return new BeamLoad();
}
