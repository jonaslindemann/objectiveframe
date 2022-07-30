#include <ofem/beam_node_bc_set.h>

using namespace ofem;

BeamNodeBCSet::BeamNodeBCSet()
{
}

BeamNodeBCSet::~BeamNodeBCSet()
{
}

BC* BeamNodeBCSet::createBC()
{
    return new BeamNodeBC();
}
