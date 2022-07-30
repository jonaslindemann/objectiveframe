#include <ofem/pipe_section.h>

#include <cmath>

using namespace ofem;

// ------------------------------------------------------------
PipeSection::PipeSection(double outerRadius, double innerRadius)
    : Section()
{
    this->setSectionType(FEM_PIPE_SECTION);
    this->setSectionSize(outerRadius, innerRadius);
}

// ------------------------------------------------------------
PipeSection::PipeSection()
{
    this->setSectionType(FEM_PIPE_SECTION);
    this->setSectionSize(0.1, 0.09);
}

// ------------------------------------------------------------
PipeSection::~PipeSection()
{
}

// ------------------------------------------------------------
void PipeSection::setSectionSize(double outerRadius, double innerRadius)
{
    double X[26], Y[26];
    int i;

    this->clear();

    m_prop[9] = outerRadius;
    m_prop[10] = innerRadius;

    X[0] = 0;
    Y[0] = outerRadius;
    X[1] = outerRadius * (1.0 / 2.0);
    Y[1] = outerRadius * (sqrt(3.0) / 2.0);
    X[2] = outerRadius * (sqrt(3.0) / 2.0);
    Y[2] = outerRadius * (1.0 / 2.0);
    X[3] = outerRadius;
    Y[3] = 0;
    X[4] = outerRadius * (sqrt(3.0) / 2.0);
    Y[4] = outerRadius * (-1.0 / 2.0);
    X[5] = outerRadius * (1.0 / 2.0);
    Y[5] = outerRadius * (-sqrt(3.0) / 2.0);
    X[6] = 0;
    Y[6] = -outerRadius;
    X[7] = outerRadius * (-1.0 / 2.0);
    Y[7] = outerRadius * (-sqrt(3.0) / 2.0);
    X[8] = outerRadius * (-sqrt(3.0) / 2.0);
    Y[8] = outerRadius * (-1.0 / 2.0);
    X[9] = -outerRadius;
    Y[9] = 0;
    X[10] = outerRadius * (-sqrt(3.0) / 2.0);
    Y[10] = outerRadius * (1.0 / 2.0);
    X[11] = outerRadius * (-1.0 / 2.0);
    Y[11] = outerRadius * (sqrt(3.0) / 2.0);
    X[12] = X[0];
    Y[12] = Y[0];
    X[13] = 0;
    Y[13] = innerRadius;
    X[14] = innerRadius * (1.0 / 2.0);
    Y[14] = innerRadius * (sqrt(3.0) / 2.0);
    X[15] = innerRadius * (sqrt(3.0) / 2.0);
    Y[15] = innerRadius * (1.0 / 2.0);
    X[16] = innerRadius;
    Y[16] = 0;
    X[17] = innerRadius * (sqrt(3.0) / 2.0);
    Y[17] = innerRadius * (-1.0 / 2.0);
    X[18] = innerRadius * (1.0 / 2.0);
    Y[18] = innerRadius * (-sqrt(3.0) / 2.0);
    X[19] = 0;
    Y[19] = -innerRadius;
    X[20] = innerRadius * (-1.0 / 2.0);
    Y[20] = innerRadius * (-sqrt(3.0) / 2.0);
    X[21] = innerRadius * (-sqrt(3.0) / 2.0);
    Y[21] = innerRadius * (-1.0 / 2.0);
    X[22] = -innerRadius;
    Y[22] = 0;
    X[23] = innerRadius * (-sqrt(3.0) / 2.0);
    Y[23] = innerRadius * (1.0 / 2.0);
    X[24] = innerRadius * (-1.0 / 2.0);
    Y[24] = innerRadius * (sqrt(3.0) / 2.0);
    X[25] = X[13];
    Y[25] = Y[13];

    for (i = 0; i < 26; i++)
        this->addPoint(X[i], Y[i]);

    this->setData();
}

// ------------------------------------------------------------
void PipeSection::setData()
{
    double d1 = 2.0 * m_prop[9];
    double d2 = 2.0 * m_prop[10];
    double pi = 3.14159265359;

    m_data[1] = pi * (pow(d1, 2) - pow(d2, 2)) / 4.0; // Area

    m_data[3] = pi * (pow(d1, 4) - pow(d2, 4)) / 64.0; // Iy

    m_data[4] = pi * (pow(d1, 4) - pow(d2, 4)) / 64.0; // Iz

    m_data[5] = pi * (pow(d1, 4) - pow(d2, 4)) / 32.0; // Kv
}

// ------------------------------------------------------------
void PipeSection::getSectionSize(double& outerRadius, double& innerRadius)
{
    outerRadius = m_prop[9];
    innerRadius = m_prop[10];
}

// ------------------------------------------------------------
void PipeSection::getExcY(double& emax, double& emin)
{
    emax = m_prop[9] / 2.0;
    emin = emax;
}

void PipeSection::calcDataFromSection()
{
    this->setData();
}

void PipeSection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    Section::setSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->setSectionSize(outerRadius, innerRadius);
}

void PipeSection::getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius)
{
    Section::getSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->getSectionSize(outerRadius, innerRadius);
}

// ------------------------------------------------------------
void PipeSection::getExcZ(double& emax, double& emin)
{
    emax = m_prop[9] / 2.0;
    emin = emax;
}
