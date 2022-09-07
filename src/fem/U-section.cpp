#include <ofem/U_section.h>

#include <cmath>

using namespace ofem;


USection::USection(double height, double UFW, double LFW, double WT, double UFT, double LFT)
    : Section()
// TODO: check and complete member initialisation list!
{
    this->setSectionType(FEM_U_SECTION);
    this->setSectionSize(height, UFW, LFW, WT, UFT, LFT);
}


USection::USection()
{
    this->setSectionType(FEM_U_SECTION);
    this->setSectionSize(0.1, 0.1, 0.1, 0.01, 0.01, 0.01);
}


USection::~USection()
{
}


void USection::setSectionSize(double height, double UFW, double LFW, double WT, double UFT, double LFT)
{
    double X[9], Y[9];
    int i;

    this->clear();

    LFW = UFW;

    m_prop[0] = height;
    m_prop[2] = UFW;
    m_prop[3] = LFW;
    m_prop[4] = WT;
    m_prop[5] = UFT;
    m_prop[6] = LFT;

    m_Xtp = (height * WT * WT / 2.0 + (LFW - WT) * LFT * ((LFW - WT) / 2.0 + WT) + (UFW - WT) * UFT * ((UFW - WT) / 2.0 + WT)) / (height * WT + (LFW - WT) * LFT + (UFW - WT) * UFT);
    m_Ytp = (height * WT * height / 2.0 + (LFW - WT) * LFT * LFT / 2.0 + (UFW - WT) * UFT * (height - UFT / 2.0)) / (height * WT + (LFW - WT) * LFT + (UFW - WT) * UFT);

    X[0] = -m_Xtp;
    Y[0] = -m_Ytp;
    X[1] = LFW - m_Xtp;
    Y[1] = -m_Ytp;
    X[2] = LFW - m_Xtp;
    Y[2] = LFT - m_Ytp;
    X[3] = WT - m_Xtp;
    Y[3] = LFT - m_Ytp;
    X[4] = WT - m_Xtp;
    Y[4] = m_Ytp - UFT;
    X[5] = UFW - m_Xtp;
    Y[5] = m_Ytp - UFT;
    X[6] = UFW - m_Xtp;
    Y[6] = m_Ytp;
    X[7] = -m_Xtp;
    Y[7] = m_Ytp;
    X[8] = X[0];
    Y[8] = Y[0];

    for (i = 0; i < 9; i++)
        this->addPoint(X[i], Y[i]);

    this->setData();
}


void USection::getSectionSize(double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT)
{
    height = m_prop[0];
    UFW = m_prop[2];
    LFW = m_prop[2];
    WT = m_prop[4];
    UFT = m_prop[5];
    LFT = m_prop[6];
}


void USection::setData()
{
    double height = m_prop[0];
    double UFW = m_prop[2];
    double LFW = m_prop[3];
    double WT = m_prop[4];
    double UFT = m_prop[5];
    double LFT = m_prop[6];

    m_data[1] = UFW * UFT + LFW * LFT + (height - UFT - LFT) * WT; // Area

    m_data[3] = (UFT * pow(UFW, 3) / 12.0 + UFW * UFT * pow(UFW / 2.0 - m_Xtp, 2)) + (LFT * pow(LFW, 3) / 12.0 + LFW * LFT * pow(LFW / 2.0 - m_Xtp, 2)) + ((height - UFT - LFT) * pow(WT, 3) / 12.0 + WT * (height - UFT - LFT) * pow(WT / 2.0 - m_Xtp, 2)); // Iy

    m_data[4] = (UFW * pow(UFT, 3) / 12.0 + UFT * UFW * pow(height - UFT / 2.0 - m_Ytp, 2)) + (LFW * pow(LFT, 3) / 12.0 + LFT * LFW * pow(LFT / 2.0 - m_Ytp, 2)) + (WT * pow(height - UFT - LFT, 3) / 12.0 + WT * (height - UFT - LFT) * pow(LFT + (height - UFT - LFT) / 2.0 - m_Ytp, 2)); // Iz

    m_data[5] = 1.1 / 3.0 * (pow(UFT, 3) * UFW + pow(LFT, 3) * LFW + pow(WT, 3) * height); // Kv
}


void USection::getExcY(double& emax, double& emin)
{
    double e1 = m_prop[1] - m_Xtp;
    double e2 = m_Xtp;

    if (e1 > e2)
    {
        emax = e1;
        emin = e2;
    }
    else
    {
        emax = e2;
        emin = e1;
    }
}

void USection::calcDataFromSection()
{
    this->setData();
}

void USection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    Section::setSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->setSectionSize(height, UFW, LFW, WT, UFT, LFT);
}

void USection::getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius)
{
    Section::getSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->getSectionSize(height, UFW, LFW, WT, UFT, LFT);
}


void USection::getExcZ(double& emax, double& emin)
{
    emax = m_prop[0] / 2.0;
    emin = emax;
}
