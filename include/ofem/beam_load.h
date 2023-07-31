#pragma once

#include <ofem/beam.h>
#include <ofem/element_load.h>

namespace ofem {

SmartPointer(BeamLoad);

class BeamLoad : public ElementLoad {
private:
    std::string m_name;
    int m_color;

public:
    BeamLoad();
    virtual ~BeamLoad();

    ClassInfo("BeamLoad", ElementLoad);
    StdFactory(BeamLoad);

    const std::string getName();
    void setName(const std::string &name);
    void setColor(int color);
    int getColor();

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
};
} // namespace ofem
