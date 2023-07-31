#include <ofem/beam_node_load_set.h>

using namespace ofem;

BeamNodeLoadSet::BeamNodeLoadSet() : NodeLoadSet()
{
}

BeamNodeLoadSet::~BeamNodeLoadSet()
{
}

Load *BeamNodeLoadSet::createLoad()
{
    return new BeamNodeLoad();
}
