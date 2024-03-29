// Implementation of: public class CFemBeamMaterial

#include <ofem/I_section.h>
#include <ofem/L_section.h>
#include <ofem/T_section.h>
#include <ofem/U_section.h>
#include <ofem/beam_material.h>
#include <ofem/pipe_section.h>
#include <ofem/rect_section.h>
#include <ofem/rhs_section.h>
#include <ofem/solid_pipe_section.h>

using namespace ofem;
using namespace std;

BeamMaterial::BeamMaterial() : Material()
{
    m_width = 0.0;
    m_height = 0.0;
    m_E = 1.0;
    m_G = 1.0;
    m_A = 1.0;
    m_Iy = 1.0;
    m_Iz = 1.0;
    m_Kv = 1.0;
    m_name = "Unnamed";
    m_color = 3;
    m_representation = -1;
}

BeamMaterial::~BeamMaterial()
{
}

void BeamMaterial::setProperties(double E, double G, double A, double Iy, double Iz, double Kv)
{
    m_E = E;
    m_G = G;
    m_A = A;
    m_Iy = Iy;
    m_Iz = Iz;
    m_Kv = Kv;
}

void BeamMaterial::getProperties(double &E, double &G, double &A, double &Iy, double &Iz, double &Kv)
{
    E = m_E;
    G = m_G;
    A = m_A;
    Iy = m_Iy;
    Iz = m_Iz;
    Kv = m_Kv;
}

void BeamMaterial::print(ostream &out)
{
    out << "Material : " << this->getName() << endl;
    out << "   E  = " << m_E << endl;
    out << "   G  = " << m_G << endl;
    out << "   A  = " << m_A << endl;
    out << "   Iy = " << m_Iy << endl;
    out << "   Iz = " << m_Iz << endl;
    out << "   Kv = " << m_Kv << endl;
}

void BeamMaterial::saveToStream(std::ostream &out)
{
    Material::saveToStream(out);
    out << this->getName() << endl;
    out << m_color << endl;
    out << m_width << " ";
    out << m_height << " ";
    out << m_representation << endl;
    out << m_E << " ";
    out << m_G << " ";
    out << m_A << " ";
    out << m_Iy << " ";
    out << m_Iz << " ";
    out << m_Kv << " " << endl;

    if (m_section != nullptr) {
        out << m_section->getSectionType() << endl;
        m_section->saveToStream(out);
    }
    else {
        out << -1 << endl;
    }
}

void BeamMaterial::readFromStream(std::istream &in)
{
    int sectionType;
    string matName;
    Material::readFromStream(in);
    in.ignore(1000, '\n');
    getline(in, matName);
    // in.getline(matName, 255, '\n');
    // in.getline(matName, 255, '\n');
    // in >> matName;
    this->setName(matName);
    in >> m_color;
    in >> m_width;
    in >> m_height;
    in >> m_representation;
    in >> m_E;
    in >> m_G;
    in >> m_A;
    in >> m_Iy;
    in >> m_Iz;
    in >> m_Kv;
    in >> sectionType;

    if (sectionType > -1) {
        switch (sectionType) {
        case FEM_I_SECTION:
            m_section = ISection::create();
            break;
        case FEM_U_SECTION:
            m_section = USection::create();
            break;
        case FEM_L_SECTION:
            m_section = LSection::create();
            break;
        case FEM_RHS_SECTION:
            m_section = RHSSection::create();
            break;
        case FEM_PIPE_SECTION:
            m_section = PipeSection::create();
            break;
        case FEM_SOLIDPIPE_SECTION:
            m_section = SolidPipeSection::create();
            break;
        case FEM_RECT_SECTION:
            m_section = RectSection::create();
            break;
        default:
            m_section = RectSection::create();
            break;
        }
        m_section->readFromStream(in);
    }
    else {
    }
}

void BeamMaterial::setSection(Section *section)
{
    m_section = SectionPtr(section);
}

void BeamMaterial::assignPropFromSection()
{
    if (m_section != nullptr) {
        this->setProperties(m_section->data(0), m_section->data(2), m_section->data(1), m_section->data(3),
                            m_section->data(4), m_section->data(5));
    }
}

void BeamMaterial::setSectionType(SectionType sectionType)
{
    switch (sectionType) {
    case ST_I:
        m_section = ISection::create();
        break;
    case ST_U:
        m_section = USection::create();
        break;
    case ST_L:
        m_section = LSection::create();
        break;
    case ST_RHS:
        m_section = RHSSection::create();
        break;
    case ST_Pipe:
        m_section = PipeSection::create();
        break;
    case ST_SolidPipe:
        m_section = SolidPipeSection::create();
        break;
    case ST_Rectangle:
        m_section = RectSection::create();
        break;
    default:
        m_section = RectSection::create();
        break;
    }
}

Section *BeamMaterial::getSection()
{
    return m_section.get();
}

void BeamMaterial::setName(const std::string &name)
{
    m_name = name;
}

const std::string BeamMaterial::getName()
{
    return m_name;
}

void BeamMaterial::setColor(int color)
{
    m_color = color;
}

int BeamMaterial::getColor()
{
    return m_color;
}
