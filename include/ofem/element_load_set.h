#pragma once

#include <ofem/load_set.h>

namespace ofem {

    SmartPointer(ElementLoadSet);

    class ElementLoadSet : public LoadSet
    {
    public:
        ElementLoadSet();
        virtual ~ElementLoadSet();

        // Class info

        ClassInfo("ElementLoadSet", LoadSet);

    protected:
        virtual Load* createLoad();
    };
}
