#ifndef _CFemRectSection_H_
#define _CFemRectSection_H_

#include "FemSection.h"

#define FEM_RECT_SECTION 4

FemSmartPointer(CFemRectSection);

/**
 * FemRectSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (Rect-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemRectSection : public CFemSection {
private:
public:
    CFemRectSection();

    /** CFemRectSection constructor */
    CFemRectSection (double width, double height);

    /** CFemRectSection destructor */
    virtual ~CFemRectSection ();

    FemClassInfo("CFemRectSection",CFemSection);

    /**
    * Get section size
    *
    * This function gives all measurements associated with the Rect-Section.
    *
    */
    void getSectionSize(double &width, double &height);

    /**
    * Set section size
    *
    * This function sets all measurements associated with the Rect-Section.
    *
    */
    void setSectionSize(double width, double height);

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






