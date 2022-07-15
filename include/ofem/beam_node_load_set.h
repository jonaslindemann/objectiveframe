#pragma once

#include <ofem/node_load_set.h>
#include <ofem/beam_node_load.h>

namespace ofem {

SmartPointer(CFEmBeamNodeLoadSet);

class BeamNodeLoadSet : public NodeLoadSet {
private:

public:
    BeamNodeLoadSet();
    virtual ~BeamNodeLoadSet();

    ClassInfo("BeamNodeLoadSet",NodeLoadSet);
protected:
    Load* createLoad();
};
}
