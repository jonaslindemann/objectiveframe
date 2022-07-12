#ifndef _CFemBeamMaterial_H_
#define _CFemBeamMaterial_H_

#include "FemMaterial.h"
#include "FemSection.h"

FemSmartPointer(CFemBeamMaterial);

class CFemBeamMaterial : public CFemMaterial {
private:
    double m_E;
    double m_G;
    double m_A;
    double m_Iy;
    double m_Iz;
    double m_Kv;
    CFemSection* m_section;
    std::string m_name;
    int m_color;
    double m_height;
    double m_width;
    int m_representation;
public:
    CFemBeamMaterial ();
    virtual ~CFemBeamMaterial ();

    FemClassInfo("CFemBeamMaterial",CFemMaterial);

    // Get/set methods

    void getProperties(double &E, double &G, double &A,
                       double &Iy, double &Iz, double &Kv);
    void setProperties(double E,double G,double A,
                       double Iy,double Iz,double Kv);
    int getColor();
    void setColor(int color);
    const std::string getName();
    void setName(const std::string& name);
    CFemSection* getSection();
    void setSection(CFemSection* section);

    void assignPropFromSection();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
