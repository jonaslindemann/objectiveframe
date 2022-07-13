// Implementation of: public class CFemSection

#include "FemSection.h"

#ifdef HAVE_CMATH
#include <cmath>
using namespace std;
#else
#include <math.h>
#endif

// ------------------------------------------------------------
FemSection::FemSection()
    :FemObject(),
     m_data{nullptr},
     m_nbrOfProps{0},
     m_autoCalc{false},
     m_sectionType{-1},
     m_prop{nullptr},
     m_nbrOfData{0},
     m_repr{-1}
{
    this->setPropSize(11);
    this->setDataSize(6);
    this->setSectionData(2.1e9, 0.1 * 0.1, 79.3e9, 1.0, 1.0, 1.0);
    this->setSectionProps(0.1, 0.1, 0.1, 0.1, 0.01, 0.01, 0.01, 0.1, 0.1, 0.1, 0.09);
}

// ------------------------------------------------------------
FemSection::~FemSection ()
{
    this->clear();
}

// ------------------------------------------------------------
void FemSection::addPoint(double x, double y)
{
    FemCoord* coord = new FemCoord();
    coord->setCoord(x, y, 0.0);
    m_coords.push_back(coord);
}

// ------------------------------------------------------------
void FemSection::clear()
{
    for (unsigned int i=0; i<m_coords.size(); i++)
        delete m_coords[i];
    m_coords.clear();
}

// ------------------------------------------------------------
void FemSection::getCoord(unsigned int idx, double &x, double &y)
{
    double z;
    if (idx<m_coords.size())
        m_coords[idx]->getCoord(x, y, z);
}

// ------------------------------------------------------------
size_t FemSection::getSize()
{
    return m_coords.size();
}

// ------------------------------------------------------------
void FemSection::getNormal(unsigned int idx, double &ex, double &ey)
{
    double x1, y1, z;
    double x2, y2, l;
    if (idx<m_coords.size()-1)
    {
        m_coords[idx]->getCoord(x1, y1, z);
        m_coords[idx+1]->getCoord(x2, y2, z);
        if (y1==y2)
        {
            ex = 0.0;
            if (x2>=x1)
                ey = 1.0;
            else
                ey = -1.0;
        }
        else if (x1==x2)
        {
            ey = 0.0;
            if (y2<y1)
                ex = 1.0;
            else
                ex = -1.0;
        }
        else
        {
            ex = -(y2-y1);
            ey = x2-x1;
            l = sqrt(pow(ex,2)+pow(ey,2));
            ex = ex/l;
            ey = ey/l;
        }
        ex = -ex;
        ey = -ey;
    }
}

// ------------------------------------------------------------
void FemSection::setPropSize(int size)
{
    m_nbrOfProps = size;
    /*if (m_prop != NULL)
    	delete [] m_prop;*/
    m_prop = new double[m_nbrOfProps];
    for (unsigned int i=0; i<m_nbrOfProps; i++)
        m_prop[i] = 0;
}

// ------------------------------------------------------------
void FemSection::getPropSize(int &size)
{
    size = m_nbrOfProps;
}

// ------------------------------------------------------------
void FemSection::getAllProps(double* &props)
{
    props = m_prop;
}

// ------------------------------------------------------------
void FemSection::setDataSize(int size)
{
    m_nbrOfData = size;
    /*if (m_prop != NULL)
    	delete [] m_prop;*/
    m_data = new double[m_nbrOfData];
    for (unsigned int i=0; i<m_nbrOfData; i++)
        m_data[i] = 0;
}

// ------------------------------------------------------------
void FemSection::getDataSize(int &size)
{
    size = m_nbrOfData;
}

// ------------------------------------------------------------
void FemSection::getAllData(double* &data)
{
    data = m_data;
}

// ------------------------------------------------------------
void FemSection::setAutoCalc(bool mode)
{
    m_autoCalc = mode;
}

// ------------------------------------------------------------
bool FemSection::autoCalc()
{
    return m_autoCalc;
}

void FemSection::calcDataFromSection()
{
}

void FemSection::saveToStream(std::ostream &out)
{
    unsigned int i;

    // Write section properties

    out << m_nbrOfProps << endl;

    for (i=0; i<m_nbrOfProps; i++)
        out << m_prop[i] << " ";
    out << endl;

    // Write material properties

    out << m_nbrOfData << endl;

    for (i=0; i<m_nbrOfData; i++)
        out << m_data[i] << " ";
    out << endl;

    // Write section geometry

    out << m_coords.size() << endl;

    for (i=0; i<m_coords.size(); i++)
        m_coords[i]->saveToStream(out);

}

void FemSection::setSectionType(int type)
{
    m_sectionType = type;
}

int FemSection::getSectionType()
{
    return m_sectionType;
}

void FemSection::readFromStream(std::istream &in)
{
    int i, propSize,  dataSize, coordSize;

    // Write section properties

    in >> propSize;
    this->setPropSize(propSize);

    for (i=0; i<propSize; i++)
        in >> m_prop[i];

    // Write material properties

    in >> dataSize;
    this->setDataSize(dataSize);

    for (i=0; i<dataSize; i++)
        in >> m_data[i];

    // Write section geometry

    in >> coordSize;
    this->clear();
    for (i=0; i<coordSize; i++)
    {
        FemCoord* coord = new FemCoord();
        coord->readFromStream(in);
        m_coords.push_back(coord);
    }
}

void FemSection::getExcY(double &emax, double &emin)
{
    emax = -1.0;
    emin = -1.0;
}

double FemSection::prop(int idx)
{
    if ((idx >= 0) && (idx < 11))
        return m_prop[idx];
    else
        return -1.0;
}

double FemSection::data(int idx)
{
    if ((idx >= 0) && (idx < 6))
        return m_data[idx];
    else
        return -1.0;
}

double FemSection::E()
{
    return m_data[0];
}

double FemSection::A()
{
    return m_data[1];
}

double FemSection::G()
{
    return m_data[2];
}

double FemSection::Iy()
{
    return m_data[3];
}

double FemSection::Iz()
{
    return m_data[4];
}

double FemSection::Kv()
{
    return m_data[5];
}

void FemSection::setE(double value)
{
    m_data[0] = value;
}

void FemSection::setA(double value)
{
    m_data[1] = value;
}

void FemSection::setG(double value)
{
    m_data[2] = value;
}

void FemSection::setIy(double value)
{
    m_data[3] = value;
}

void FemSection::setIz(double value)
{
    m_data[4] = value;
}

void FemSection::setKv(double value)
{
    m_data[5] = value;
}

void FemSection::setSectionData(double E, double A, double G, double Iy, double Iz, double Kv)
{
    m_data[0] = E;
    m_data[1] = A;
    m_data[2] = G;
    m_data[3] = Iy;
    m_data[4] = Iz;
    m_data[5] = Kv;
}

void FemSection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    m_prop[0] = height;
    m_prop[1] = width;
    m_prop[2] = UFW;
    m_prop[3] = LFW;
    m_prop[4] = WT;
    m_prop[5] = UFT;
    m_prop[6] = LFT;
    m_prop[7] = ULFW;
    m_prop[8] = LLFW;
    m_prop[9] = outerRadius;
    m_prop[10] = innerRadius;
}

void FemSection::getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius)
{
    height = m_prop[0];
    width = m_prop[1];
    UFW = m_prop[2];
    LFW = m_prop[3];
    WT = m_prop[4];
    UFT = m_prop[5];
    LFT = m_prop[6];
    ULFW = m_prop[7];
    LLFW = m_prop[8];
    outerRadius = m_prop[9];
    innerRadius = m_prop[10];
}

void FemSection::getExcZ(double &emax, double &emin)
{
    emax = -1.0;
    emin = -1.0;
}
