#include <ofem/rhs_section.h>

#include <cmath>

using namespace ofem;

RHSSection::RHSSection(double height, double width, double WT) : Section()
// TODO: check and complete member initialisation list!
{
    this->setSectionType(FEM_RHS_SECTION);
    this->setSectionSize(height, width, WT);
}

RHSSection::RHSSection()
{
    this->setSectionType(FEM_RHS_SECTION);
    this->setPropSize(11);
    this->setDataSize(6);
    this->setSectionSize(0.1, 0.1, 0.01);
}

RHSSection::~RHSSection()
{
}

void RHSSection::setSectionSize(double height, double width, double WT)
{
    double X[10], Y[10];
    int i;

    this->clear();

    m_prop[0] = height;
    m_prop[1] = width;
    m_prop[4] = WT;

    X[0] = -width / 2;
    Y[0] = -height / 2;
    X[1] = width / 2;
    Y[1] = -height / 2;
    X[2] = width / 2;
    Y[2] = height / 2;
    X[3] = -width / 2;
    Y[3] = height / 2;
    X[4] = X[0];
    Y[4] = Y[0];
    X[5] = WT - width / 2;
    Y[5] = WT - height / 2;
    X[6] = width / 2 - WT;
    Y[6] = WT - height / 2;
    X[7] = width / 2 - WT;
    Y[7] = height / 2 - WT;
    X[8] = WT - width / 2;
    Y[8] = height / 2 - WT;
    X[9] = X[5];
    Y[9] = Y[5];

    for (i = 0; i < 9; i++)
        this->addPoint(X[i], Y[i]);

    this->setData();
}

void RHSSection::getSectionSize(double &height, double &width, double &WT)
{
    height = m_prop[0];
    width = m_prop[1];
    WT = m_prop[4];
}

void RHSSection::setData()
{
    double height = m_prop[0];
    double width = m_prop[1];
    double WT = m_prop[4];
    // double temp, c1;

    m_data[1] = height * width - (height - 2.0 * WT) * (width - 2.0 * WT); // Area

    m_data[3] = height * pow(width, 3) / 12.0 - (height - 2.0 * WT) * pow(width - 2.0 * WT, 3) / 12.0; // Iy

    m_data[4] = width * pow(height, 3) / 12.0 - (width - 2.0 * WT) * pow(height - 2.0 * WT, 3) / 12.0; // Iz

    /*if (width>height)
    {
        temp=height;
        height=width;
        width=temp;
    }
    c1 = 0.20 + 3.7e-2 * log(height/width);	//home made polynom, should be improved ...
    m_data[5] = c1*height*pow(width,3) - c1*(height-2.0*WT)*pow(width-2.0*WT,3);			//Kv*/
    m_data[5] = 0;
}

void RHSSection::getExcY(double &emax, double &emin)
{
    emax = m_prop[1] / 2.0;
    emin = emax;
}

void RHSSection::calcDataFromSection()
{
    this->setData();
}

void RHSSection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT,
                                 double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    Section::setSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->setSectionSize(height, width, WT);
}

void RHSSection::getSectionProps(double &width, double &height, double &UFW, double &LFW, double &WT, double &UFT,
                                 double &LFT, double &ULFW, double &LLFW, double &outerRadius, double &innerRadius)
{
    Section::getSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->getSectionSize(height, width, WT);
}

void RHSSection::getExcZ(double &emax, double &emin)
{
    emax = m_prop[0] / 2.0;
    emin = emax;
}
