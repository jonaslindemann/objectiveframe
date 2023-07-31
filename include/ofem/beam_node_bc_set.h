#pragma once

#include <ofem/beam_node_bc.h>
#include <ofem/node_bc_set.h>

namespace ofem {

SmartPointer(BeamNodeBCSet);

class BeamNodeBCSet : public NodeBCSet {
private:
public:
    BeamNodeBCSet();
    virtual ~BeamNodeBCSet();

    ClassInfo("BeamNodeBCSet", NodeBCSet);
    StdFactory(BeamNodeBCSet);

protected:
    virtual BC *createBC() override;
};
} // namespace ofem
