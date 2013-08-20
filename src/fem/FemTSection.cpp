// Implementation of: public class CFemTSection

#include "FemTSection.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

// ------------------------------------------------------------
CFemTSection::CFemTSection (double width, double height, double WT, double UFT, double ULFW)
    :CFemSection()
{
    this->setSectionType(FEM_T_SECTION);
    this->setPropSize(11);
    this->setDataSize(6);
    this->setSectionSize(width, height, WT, UFT, ULFW);
}

// ------------------------------------------------------------
CFemTSection::~CFemTSection ()
{
}

// ------------------------------------------------------------
void CFemTSection::setSectionSize(double width, double height, double WT, double UFT, double ULFW)
{
    double X[9], Y[9];
    int i;

    this->clear();

    ULFW = width/2.0 - WT/2.0;

    m_prop[0] = height;
    m_prop[1] = width;
    m_prop[4] = WT;
    m_prop[5] = UFT;
    m_prop[7] = ULFW;

    m_Ytp=(WT*(height-UFT)*(height-UFT)/2.0+width*UFT*(height-UFT/2.0))/
          (WT*(height-UFT)+width*UFT);
    m_Xtp=(width*UFT*width/2.0+(height-UFT)*WT*(ULFW+WT/2.0))/
          (width*UFT+(height-UFT)*WT);

    X[0] = ULFW - m_Xtp;
    Y[0] = -m_Ytp;
    X[1] = ULFW + WT - m_Xtp;
    Y[1] = -m_Ytp;
    X[2] = ULFW + WT - m_Xtp;
    Y[2] = height - UFT - m_Ytp;
    X[3] = width - m_Xtp;
    Y[3] = height - UFT -m_Ytp;
    X[4] = width - m_Xtp;
    Y[4] = height - m_Ytp;
    X[5] = -m_Xtp;
    Y[5] = height - m_Ytp;
    X[6] = -m_Xtp;
    Y[6] = height - UFT - m_Ytp;
    X[7] = ULFW - m_Xtp;
    Y[7] = height - UFT - m_Ytp;
    X[8] = X[0];
    Y[8] = Y[0];

    for (i=0; i<9; i++)
        this->addPoint(X[i],Y[i]);

    this->setData();
}

// ------------------------------------------------------------
void CFemTSection::getSectionSize(double &width, double &height, double &WT, double &UFT, double &ULFW)
{
    width = m_prop[1];
    height = m_prop[0];
    WT = m_prop[4];
    UFT = m_prop[5];
    ULFW = m_prop[7];
}

// ------------------------------------------------------------
void CFemTSection::setData()
{
    double width = m_prop[1];
    double height = m_prop[0];
    double WT = m_prop[4];
    double UFT = m_prop[5];
    double ULFW = m_prop[7];

    m_data[1] = width*UFT + (height-UFT)*WT;	//Area

    m_data[3] = ((height-UFT)*pow(WT,3)/12.0 + WT*(height-UFT)*pow(ULFW+WT/2.0-m_Xtp,2)) +
                (UFT*pow(width,3)/12.0 + width*UFT*pow(width/2.0-m_Xtp,2));	//Iy

    m_data[4] = (WT*pow(height-UFT,3)/12.0 + (height-UFT)*WT*pow((height-UFT)/2.0-m_Ytp,2)) +
                (width*pow(UFT,3)/12.0 + UFT*width*pow(height-UFT/2.0-m_Ytp,2));	//Iz

    m_data[5] = 1.1/3.0*(pow(WT,3)*height + pow(UFT,3)*width);	//Kv
}

// ------------------------------------------------------------
void CFemTSection::getExcY(double &emax, double &emin)
{
    emax = m_prop[1] / 2.0;
    emin = emax;
}

// ------------------------------------------------------------
void CFemTSection::getExcZ(double &emax, double &emin)
{
    double e1 = m_prop[0] - m_Ytp;
    double e2 = m_Ytp;

    if (e1>e2)
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
