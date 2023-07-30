#pragma once

#include <ofem/node_bc.h>

namespace ofem
{

SmartPointer(BeamNodeBC);

class BeamNodeBC : public NodeBC
{
private:
    int m_color;
    std::string m_name;

public:
    BeamNodeBC();
    virtual ~BeamNodeBC();

    ClassInfo("BeamNodeBC", NodeBC);
    StdFactory(BeamNodeBC);

    int getColor();
    void setColor(int color);
    const std::string getName();
    void setName(const std::string& name);

    void readFromStream(std::istream& in) override;
    void saveToStream(std::ostream& out) override;
};
}
