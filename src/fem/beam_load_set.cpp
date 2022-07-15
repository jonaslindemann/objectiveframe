#include <ofem/beam_load_set.h>
#include <ofem/beam_load.h>

using namespace ofem;

// ------------------------------------------------------------
BeamLoadSet::BeamLoadSet()
    :ElementLoadSet()
{

}

// ------------------------------------------------------------
BeamLoadSet::~BeamLoadSet()
{

}

// ------------------------------------------------------------
Load* BeamLoadSet::createLoad()
{
    return new BeamLoad();
}

