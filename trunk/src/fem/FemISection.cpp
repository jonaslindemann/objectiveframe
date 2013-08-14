// Implementation of: public class CFemISection

#include "FemISection.h"

#ifdef HAVE_CMATH
#include <cmath>
#else
#include <math.h>
#endif

// ------------------------------------------------------------
CFemISection::CFemISection (double height, double UFW, double LFW, double WT,
						   double UFT, double LFT, double ULFW, double LLFW):CFemSection()

{
	this->setSectionType(FEM_I_SECTION);
	this->setPropSize(11);
	this->setDataSize(6);
	this->setSectionSize(height, UFW, LFW, WT, UFT, LFT, ULFW, LLFW);
}

// ------------------------------------------------------------
CFemISection::CFemISection()
{
	this->setSectionType(FEM_I_SECTION);
	this->setPropSize(11);
	this->setDataSize(6);
	this->setSectionSize(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

// ------------------------------------------------------------
CFemISection::~CFemISection ()
{
}

// ------------------------------------------------------------
void CFemISection::setSectionSize(double height, double UFW, double LFW, double WT,
								  double UFT, double LFT, double ULFW, double LLFW)
{
	double X[13], Y[13];
	int i;

	this->clear();

	m_prop[0] = height;
	m_prop[2] = UFW;
	m_prop[3] = LFW;
	m_prop[4] = WT;
	m_prop[5] = UFT;
	m_prop[6] = LFT;
	ULFW = UFW/2.0 - WT/2.0;
	LLFW = LFW/2.0 - WT/2.0;
	m_prop[7] = ULFW;
	m_prop[8] = LLFW;

	m_Ytp = (LFW*LFT*(LFT/2) + WT*(height-LFT-UFT)*((height-LFT-UFT)/2+LFT) + UFT*UFW*(height-UFT/2)) /
			(LFW*LFT + WT*(height-LFT-UFT) + UFT*UFW);
	m_Xtp = (LFW*LFT*(LFW/2) + WT*(height-LFT-UFT)*(LLFW+WT/2) + UFW*UFT*(LLFW-ULFW+UFW/2)) /
			(LFW*LFT + WT*(height-LFT-UFT) + UFW*UFT);

	X[0] = -m_Xtp;
	Y[0] = -m_Ytp;
	X[1] = LFW - m_Xtp;
	Y[1] = -m_Ytp;
	X[2] = LFW - m_Xtp;
	Y[2] = LFT - m_Ytp;
	X[3] = LLFW + WT - m_Xtp;
	Y[3] = LFT - m_Ytp;
	X[4] = LLFW + WT - m_Xtp;
	Y[4] = height - UFT -m_Ytp;
	X[5] = LLFW - ULFW + UFW - m_Xtp;
	Y[5] = height - UFT -m_Ytp;
	X[6] = LLFW - ULFW + UFW - m_Xtp;
	Y[6] = height - m_Ytp;
	X[7] = LLFW - ULFW - m_Xtp;
	Y[7] = height - m_Ytp;
	X[8] = LLFW - ULFW - m_Xtp;
	Y[8] = height - UFT -m_Ytp;
	X[9] = LLFW - m_Xtp;
	Y[9] = height - UFT -m_Ytp;
	X[10] = LLFW - m_Xtp;
	Y[10] = LFT - m_Ytp;
	X[11] = -m_Xtp;
	Y[11] = LFT - m_Ytp;
	X[12] = X[0];
	Y[12] = Y[0];

	for (i=0; i<13; i++)
		this->addPoint(X[i],Y[i]);

	this->setData();
}

// ------------------------------------------------------------
void CFemISection::getSectionSize(double &height, double &UFW, double &LFW, double &WT, 
								  double &UFT, double &LFT, double &ULFW, double &LLFW)
{
	height = m_prop[0];
	UFW = m_prop[2];
	LFW = m_prop[3];
	WT = m_prop[4];
	UFT = m_prop[5];
	LFT = m_prop[6];
	ULFW = m_prop[7];
	LLFW = m_prop[8];
}

// ------------------------------------------------------------
void CFemISection::setData()
{
	double height = m_prop[0];
	double UFW = m_prop[2];
	double LFW = m_prop[3];
	double WT = m_prop[4];
	double UFT = m_prop[5];
	double LFT = m_prop[6];
	double ULFW = m_prop[7];
	double LLFW = m_prop[8];

	m_data[1] = UFW*UFT + LFW*LFT + (height-UFT-LFT)*WT;	//Area
	
	m_data[3] = (UFT*pow(UFW,3)/12.0 + UFT*UFW*pow(LLFW-ULFW+UFW/2.0-m_Xtp,2)) +
				(LFT*pow(LFW,3)/12.0 + LFT*LFW*pow(LFW/2.0-m_Xtp,2)) + 
				((height-LFT-UFT)*pow(WT,3)/12.0 + (height-LFT-UFT)*WT*pow(LLFW+WT/2.0-m_Xtp,2));	//Iy													//Iy
	
	m_data[4] = (UFW*pow(UFT,3)/12.0 + UFW*UFT*pow(height-UFT/2.0-m_Ytp,2)) + 
				(LFW*pow(LFT,3)/12.0 + LFW*LFT*pow(LFT/2.0-m_Ytp,2)) + 
				(WT*pow(height-UFT-LFT,3)/12.0 + (height-UFT-LFT)*WT*pow(LFT+(height-UFT-LFT)/2-m_Ytp,2));	//Iz	

	m_data[5] = 1.15/3.0*(pow(UFT,3)*UFW + pow(LFT,3)*LFW + pow(WT,3)*height);	//Kv
}

// ------------------------------------------------------------
void CFemISection::getExcY(double &emax, double &emin)
{
	emax = m_prop[1] / 2.0;
	emin = emax;
}

// ------------------------------------------------------------
void CFemISection::getExcZ(double &emax, double &emin)
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
