#pragma once

#include <ivf/Base.h>
#include <ivf/Material.h>

namespace vfem {

IvfSmartPointer(ColorTable);

class ColorTable : public ivf::Base {
private:
    float m_colorTable[256][3];

public:
    ColorTable();
    virtual ~ColorTable();

    IvfClassInfo("ColorTable", ivf::Base);

    static ColorTablePtr create();

    void setColor(int idx, float r, float g, float b);
    void assignColor(int idx, ivf::Material *material);
    void getColor(int idx, float &r, float &g, float &b);
};
} // namespace vfem
