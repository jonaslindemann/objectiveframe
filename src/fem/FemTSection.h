#ifndef _CFemTSection_H_
#define _CFemTSection_H_

#include "FemSection.h"

#define FEM_T_SECTION 7

FemSmartPointer(CFemTSection);

/**
 * FemTSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific 
 * type of crossection (T-Section). This class stores all measurements and 
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv) 
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemTSection : public CFemSection {
private:
	double m_Ytp;
	double m_Xtp;
public:
	/** CFemTSection constructor */
	CFemTSection (double width, double height, double WT, double UFT, double ULFW);

	/** CFemTSection destructor */
	virtual ~CFemTSection ();

	FemClassInfo("CFemTSection",CFemSection);

	/**
	* Get section size
	*
	* This function gives all measurements associated with the T-Section.
	* 
	*/
	void getSectionSize(double &width, double &height, double &WT, double &UFT, double &ULFW);

	/**
	* Set section size
	*
	* This function sets all measurements associated with the T-Section.
	* 
	*/
	void setSectionSize(double width, double height, double WT, double UFT, double ULFW);

	/**
	* Set data
	*
	* This function (re-) calculates and sets section properties (A, Iy, Iz, Kv).
	* 
	*/
	void setData();
	
	virtual void getExcZ(double &emax, double &emin);
	virtual void getExcY(double &emax, double &emin);
};
#endif


	

	

