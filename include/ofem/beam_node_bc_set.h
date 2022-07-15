#pragma once

#include <ofem/beam_node_bc.h>
#include <ofem/node_bc_set.h>

namespace ofem {

    SmartPointer(CFemBeamNodeSetBCSet);

    class BeamNodeBCSet : public NodeBCSet {
    private:

    public:
        BeamNodeBCSet();
        virtual ~BeamNodeBCSet();

        ClassInfo("BeamNodeBCSet", NodeBCSet);
    protected:
        BC* createBC();
    };
}
