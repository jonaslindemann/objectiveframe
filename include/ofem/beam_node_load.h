#pragma once

#include <ofem/node_load.h>

namespace ofem
{

SmartPointer(BeamNodeLoad);

class BeamNodeLoad : public NodeLoad
{
private:
    int m_color;
    std::string m_name;

public:
    BeamNodeLoad();
    virtual ~BeamNodeLoad();

    ClassInfo("BeamNodeLoad", NodeLoad);

    void setColor(int color);
    int getColor();

    void setName(const std::string& name);
    const std::string getName();

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
};
}
