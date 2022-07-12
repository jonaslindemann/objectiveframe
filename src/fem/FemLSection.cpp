// Implementation of: public class CFemLSection

#include "FemLSection.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

// ------------------------------------------------------------
CFemLSection::CFemLSection (double height, double width, double WT, double LFT)
    :CFemSection()
{
    this->setSectionType(FEM_L_SECTION);
    this->setSectionSize(height, width, WT, LFT);
}

// ------------------------------------------------------------
CFemLSection::CFemLSection()
{
    this->setSectionType(FEM_L_SECTION);
    this->setPropSize(11);
    this->setDataSize(6);
    this->setSectionSize(0.0, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------
CFemLSection::~CFemLSection ()
{
}

// ------------------------------------------------------------
void CFemLSection::setSectionSize(double width, double height, double WT, double LFT)
{
    double X[7], Y[7];
    int i;

    this->clear();

    m_prop[0] = height;
    m_prop[1] = width;
    m_prop[4] = WT;
    m_prop[6] = LFT;

    m_Xtp = (width*LFT*(width/2) + (height-LFT)*WT*(WT/2)) /
            (width*LFT + (height-LFT)*WT);

    m_Ytp = (width*LFT*(LFT/2) + WT*(height-LFT)*((height-LFT)/2+LFT)) /
            (width*LFT + WT*(height-LFT));

    X[0] = -m_Xtp;
    Y[0] = -m_Ytp;
    X[1] = width - m_Xtp;
    Y[1] = -m_Ytp;
    X[2] = width - m_Xtp;
    Y[2] = LFT - m_Ytp;
    X[3] = WT - m_Xtp;
    Y[3] = LFT - m_Ytp;
    X[4] = WT - m_Xtp;
    Y[4] = height - m_Ytp;
    X[5] = -m_Xtp;
    Y[5] = height - m_Ytp;
    X[6] = X[0];
    Y[6] = Y[0];

    for (i=0; i<7; i++)
        this->addPoint(X[i], Y[i]);

    this->setData();
}

// ------------------------------------------------------------
void CFemLSection::getSectionSize(double &width, double &height, double &WT, double &LFT)
{
    height = m_prop[0];
    width = m_prop[1];
    WT = m_prop[4];
    LFT = m_prop[6];
}

// ------------------------------------------------------------
void CFemLSection::setData()
{
    double height = m_prop[0];
    double width = m_prop[1];
    double WT = m_prop[4];
    double LFT = m_prop[6];

    /*printf("\ncy: %e",m_Xtp);
    printf("\ncz: %e",m_Ytp);

    printf("\n\nheight: %e",height);
    printf("\nwidth: %e",width);
    printf("\nWT: %e",WT);
    printf("\nLFT: %e",LFT);*/

    m_data[1] = height*WT + (width-WT)*LFT;	//Area
    //printf("\n\nArea: %e",m_data[1]);

    m_data[3] = (LFT*pow(width,3)/12.0 + width*LFT*pow(width/2.0-m_Xtp,2)) +
                ((height-LFT)*pow(WT,3)/12.0 + WT*(height-LFT)*pow(WT/2.0-m_Xtp,2));	//Iy
    //printf("\nIy: %e",m_data[3]);

    m_data[4] = (width*pow(LFT,3)/12.0 + LFT*width*pow(LFT/2.0-m_Ytp,2)) +
                (WT*pow(height-LFT,3)/12.0 + (height-LFT)*WT*pow(LFT+(height-LFT)/2.0-m_Ytp,2));	//Iz
    //printf("\nIz: %e",m_data[4]);

    m_data[5] = 1.0/3.0*(pow(WT,3)*height + pow(LFT,3)*width);	//Kv
    //printf("\nKv: %e",m_data[5]);
}

void CFemLSection::calcDataFromSection()
{
    this->setData();
}

void CFemLSection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    CFemSection::setSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->setSectionSize(width, height, WT, LFT);
}

void CFemLSection::getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius)
{
    CFemSection::getSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->getSectionSize(width, height, WT, LFT);
}


