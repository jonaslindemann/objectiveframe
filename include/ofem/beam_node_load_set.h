#pragma once

#include <ofem/beam_node_load.h>
#include <ofem/node_load_set.h>

namespace ofem
{

SmartPointer(CFEmBeamNodeLoadSet);

class BeamNodeLoadSet : public NodeLoadSet
{
private:
public:
    BeamNodeLoadSet();
    virtual ~BeamNodeLoadSet();

    ClassInfo("BeamNodeLoadSet", NodeLoadSet);

protected:
    virtual Load* createLoad() override;
};
}
