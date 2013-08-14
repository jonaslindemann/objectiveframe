#ifndef _CFemSection_H_
#define _CFemSection_H_

#include "FemObject.h"
#include "FemCoord.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(CFemSection);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

/**
 * FemSection class.
 *
 * This class is a generic crossection class that contains variables 
 * for all sorts of measurements and properties concerning the crossection.
 * This class is supposed to be used as an abstract class.
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemSection : public CFemObject {
private:
	std::vector<CFemCoord*> m_coords;
	unsigned int m_nbrOfProps;
	unsigned int m_nbrOfData;
	int m_sectionType;
public:
	void readFromStream(std::istream &in);
	int getSectionType();
	void setSectionType(int type);
	void saveToStream(std::ostream &out);
	int m_repr;
	bool m_autoCalc;
	double* m_prop;
	/*
	m_prop[0]  = height
	m_prop[1]  = width
	m_prop[2]  = UFW	(Upper Flange Width)
	m_prop[3]  = LFW	(Lower Flange Width)
	m_prop[4]  = WT		(Waist Thickness)
	m_prop[5]  = UFT	(Upper Flange Thickness)
	m_prop[6]  = LFT	(Lower Flange Thickness)
	m_prop[7]  = ULFW	(Upper Left Flange Width)
	m_prop[8]  = LLFW	(Lower Left Flange Width)
	m_prop[9]  = OuterRadius
	m_prop[10] = InnerRadius
	*/
	double* m_data;
	/*
	m_data[0]  = Young´s modulus (E)
	m_data[1]  = Area (A)
	m_data[2]  = Shear modulus (G)
	m_data[3]  = Moment of inertia, y (Iy)
	m_data[4]  = Moment of inertia, z (Iz)
	m_data[5]  = Saint-Venant const. (Kv)
	*/


	/** CFemSection constructor */
	CFemSection ();

	/** CFemSection destructor */	
	virtual ~CFemSection ();

	FemClassInfo("CFemSection",CFemObject);

	/**
	* Get normal
	*
	* Returns ex and ey for a specific normal answering to idx.
	*
	*/
	void getNormal(unsigned int idx, double &ex, double &ey);
	
	/**
	* Add point
	*
	* This method is used to create a crossection by building it
	* point by point. The last point must same as one of the other
	* points to make a continuos crossection.
	*
	*/
	void addPoint(double x, double y);
	
	/**
	* Clear 
	*
	* This method clears the visual representation of the current crossection. 
	*
	*/
	void clear();
	
	/**
	* Set properties size
	*
	* This method sets the number of different measurements available for 
	* creating a crossection. 
	*
	*/
	void setPropSize(int size);
	
	/**
	* Get properties size
	*
	* This method returns the number of different measurements available for 
	* creating a crossection. 
	*
	*/
	void getPropSize(int &size);
	
	/**
	* Get all properties
	*
	* This method returns a pointer to the variable containing all measurements. 
	*
	*/
	void getAllProps(double* &props);
	
	/**
	* Set data size
	*
	* This method sets the number of different properties available for 
	* the crossection, such as Area, moment of inertia, st.Venant constant etc. 
	*
	*/
	void setDataSize(int size);
	
	/**
	* Set data size
	*
	* This method returns the number of different properties available for 
	* the crossection, such as Area, moment of inertia, st.Venant constant etc. 
	*
	*/
	void getDataSize(int &size);
	
	/**
	* Get all data
	*
	* This method returns a pointer to the variable containing all properties. 
	*
	*/
	void getAllData(double* &data);
	
	/**
	* Get cooordinate
	*
	* This method returns the X- and Y-composant for a specific coordinate
	* answering to idx. 
	*
	*/
	void getCoord(unsigned int idx, double &x, double &y);
	
	/**
	* Set auto calculate
	*
	* This method sets the variable m_autoCalc that determines wheter the class itself 
	* should calculate section data or if the user should do it. 
	*
	*/
	void setAutoCalc(bool mode);
	
	/**
	* Auto calculate
	*
	* This method returns the state of the variable m_autoCalc. 
	*
	*/
	bool autoCalc();
	
	/**
	* Get size
	*
	* This method returns the the number of points used to create the crossection.. 
	*
	*/
	unsigned int getSize();
	
	virtual void getExcZ(double& emax, double &emin);
	virtual void getExcY(double &emax, double &emin);
};
#endif
