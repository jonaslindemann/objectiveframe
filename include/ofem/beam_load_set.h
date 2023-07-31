#pragma once

#include <ofem/element_load_set.h>
#include <ofem/load.h>

namespace ofem {

SmartPointer(BeamLoadSet);

class BeamLoadSet : public ElementLoadSet {
public:
    BeamLoadSet();
    virtual ~BeamLoadSet();

    ClassInfo("BeamLoadSet", ElementLoadSet);
    StdFactory(BeamLoadSet);

protected:
    virtual Load *createLoad() override;
};

} // namespace ofem
