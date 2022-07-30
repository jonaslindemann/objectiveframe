#pragma once

#include <ofem/section.h>

namespace ofem
{

SmartPointer(TubeSection);

class TubeSection : public Section
{
public:
    TubeSection();
    virtual ~TubeSection();

    ClassInfo("TubeSection", Section);
};
}
