// Implementation of: public class CFemRectSection

#include "FemRectSection.h"
#include <math.h>

// ------------------------------------------------------------
FemRectSection::FemRectSection (double width, double height)
    :FemSection()
{
    this->setSectionType(FEM_RECT_SECTION);
    this->setSectionSize(width, height);
}

// ------------------------------------------------------------
FemRectSection::FemRectSection()
{
    this->setSectionType(FEM_RECT_SECTION);
    this->setSectionSize(0.0, 0.0);
}

// ------------------------------------------------------------
FemRectSection::~FemRectSection ()
{
}

// ------------------------------------------------------------
void FemRectSection::setSectionSize(double width, double height)
{
    double X[5], Y[5];
    int i;

    this->clear();

    m_prop[0] = height;
    m_prop[1] = width;

    X[0] = -width/2.0;
    Y[0] = -height/2.0;
    X[1] = width/2.0;
    Y[1] = -height/2.0;
    X[2] = width/2.0;
    Y[2] = height/2.0;
    X[3] = -width/2.0;
    Y[3] = height/2.0;
    X[4] = X[0];
    Y[4] = Y[0];

    for (i=0; i<5; i++)
        this->addPoint(X[i],Y[i]);

    this->setData();
}

// ------------------------------------------------------------
void FemRectSection::getSectionSize(double &width, double &height)
{
    height = m_prop[0];
    width  = m_prop[1];
}

// ------------------------------------------------------------
void FemRectSection::setData()
{
    double h,w,temp,c1;

    h  = m_prop[0];
    w  = m_prop[1];

    m_data[1] = w*h;			//Area
    m_data[3] = h*pow(w,3)/12.0;	//Iy
    m_data[4] = w*pow(h,3)/12.0;	//Iz

    if (w>h)
    {
        temp=h;
        h=w;
        w=temp;
    }

    if (h/w > 2.0)		//home made polynoms, should be improved ...
        c1 = 0.20 + 3.7e-2 * log(h/w);
    else
        c1 = 0.14 + 0.13 * log(h/w);

    m_data[5] = c1*h*pow(w,3);			//Kv
}

// ------------------------------------------------------------
void FemRectSection::getExcY(double &emax, double &emin)
{
    emax = m_prop[1] / 2.0;
    emin = emax;
}

void FemRectSection::calcDataFromSection()
{
    this->setData();
}

void FemRectSection::setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius)
{
    FemSection::setSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->setSectionSize(width, height);
}

void FemRectSection::getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT, double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius)
{
    FemSection::getSectionProps(width, height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW, outerRadius, innerRadius);
    this->getSectionSize(width, height);
}

// ------------------------------------------------------------
void FemRectSection::getExcZ(double &emax, double &emin)
{
    emax = m_prop[0] / 2.0;
    emin = emax;
}
