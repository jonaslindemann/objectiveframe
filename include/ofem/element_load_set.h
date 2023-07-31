#pragma once

#include <ofem/load_set.h>

namespace ofem {

SmartPointer(ElementLoadSet);

class ElementLoadSet : public LoadSet {
public:
    ElementLoadSet();
    virtual ~ElementLoadSet();

    // Class info

    ClassInfo("ElementLoadSet", LoadSet);
    StdFactory(ElementLoadSet);

protected:
    virtual Load *createLoad() override;
};
} // namespace ofem
