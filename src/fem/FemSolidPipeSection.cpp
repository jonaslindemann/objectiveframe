// Implementation of:  class CFemSolidPipeSection

#include "FemSolidPipeSection.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_CMATH
#include <cmath> 
#else
#include <math.h>
#endif

// ------------------------------------------------------------
CFemSolidPipeSection::CFemSolidPipeSection (double outerRadius)
		:CFemSection()
{
	this->setSectionType(FEM_SOLIDPIPE_SECTION);
	this->setPropSize(11);
	this->setDataSize(6);
	this->setSectionSize(outerRadius);
}

// ------------------------------------------------------------
CFemSolidPipeSection::CFemSolidPipeSection()
{
	this->setSectionType(FEM_SOLIDPIPE_SECTION);
	this->setPropSize(11);
	this->setDataSize(6);
	this->setSectionSize(0.0);
}

// ------------------------------------------------------------
CFemSolidPipeSection::~CFemSolidPipeSection ()
{
}

// ------------------------------------------------------------
void CFemSolidPipeSection::setSectionSize(double outerRadius)
{
	double X[13], Y[13];
	int i;

	this->clear();

	m_prop[9] = outerRadius;

	X[0] = 0;
	Y[0] = outerRadius;
	X[1] = outerRadius*(1.0/2.0);
	Y[1] = outerRadius*(sqrt(3.0)/2.0);
	X[2] = outerRadius*(sqrt(3.0)/2.0);
	Y[2] = outerRadius*(1.0/2.0);
	X[3] = outerRadius;
	Y[3] = 0;
	X[4] = outerRadius*(sqrt(3.0)/2.0);
	Y[4] = outerRadius*(-1.0/2.0);
	X[5] = outerRadius*(1.0/2.0);
	Y[5] = outerRadius*(-sqrt(3.0)/2.0);
	X[6] = 0;
	Y[6] = -outerRadius;
	X[7] = outerRadius*(-1.0/2.0);
	Y[7] = outerRadius*(-sqrt(3.0)/2.0);
	X[8] = outerRadius*(-sqrt(3.0)/2.0);
	Y[8] = outerRadius*(-1.0/2.0);
	X[9] = -outerRadius;
	Y[9] = 0;
	X[10] = outerRadius*(-sqrt(3.0)/2.0);
	Y[10] = outerRadius*(1.0/2.0);
	X[11] = outerRadius*(-1.0/2.0);
	Y[11] = outerRadius*(sqrt(3.0)/2.0);
	X[12] = X[0];
	Y[12] = Y[0];

	for (i=0; i<13; i++)
		this->addPoint(X[i], Y[i]);

	this->setData();
}

// ------------------------------------------------------------
void CFemSolidPipeSection::setData()
{
	double d = 2.0*m_prop[9];
	double pi = 3.14159265359;

	m_data[1] = pi*pow(d,2)/4.0;	//Area
	
	m_data[3] = pi*pow(d,4)/64.0;	//Iy
	
	m_data[4] = pi*pow(d,4)/64.0;	//Iz

	m_data[5] = pi*pow(d,4)/32.0;	//Kv

}

// ------------------------------------------------------------
void CFemSolidPipeSection::getSectionSize(double &outerRadius)
{
	outerRadius = m_prop[9];
}

// ------------------------------------------------------------
void CFemSolidPipeSection::getExcY(double &emax, double &emin)
{
	emax = m_prop[9] / 2.0;
	emin = emax;
}

// ------------------------------------------------------------
void CFemSolidPipeSection::getExcZ(double &emax, double &emin)
{
	emax = m_prop[9] / 2.0;
	emin = emax;
}
