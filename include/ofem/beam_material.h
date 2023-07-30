#pragma once

#include <ofem/material.h>
#include <ofem/section.h>

namespace ofem
{

SmartPointer(BeamMaterial);

enum SectionType
{
    ST_I = 1,
    ST_U = 8,
    ST_L = 2,
    ST_RHS = 5,
    ST_Pipe = 3,
    ST_SolidPipe = 6,
    ST_Rectangle = 4
};

class BeamMaterial : public Material
{
private:
    double m_E;
    double m_G;
    double m_A;
    double m_Iy;
    double m_Iz;
    double m_Kv;
    SectionPtr m_section;
    std::string m_name;
    int m_color;
    double m_height;
    double m_width;
    int m_representation;

public:
    BeamMaterial();
    virtual ~BeamMaterial();

    ClassInfo("BeamMaterial", Material);
    StdFactory(BeamMaterial);

    // Get/set methods

    void getProperties(double& E, double& G, double& A,
        double& Iy, double& Iz, double& Kv);
    void setProperties(double E, double G, double A,
        double Iy, double Iz, double Kv);
    int getColor();
    void setColor(int color);
    const std::string getName();
    void setName(const std::string& name);
    Section* getSection();
    void setSection(Section* section);

    void assignPropFromSection();

    void setSectionType(SectionType sectionType);

    // IO Methods

    virtual void readFromStream(std::istream& in) override;
    virtual void saveToStream(std::ostream& out) override;
    virtual void print(std::ostream& out) override;
};
}
