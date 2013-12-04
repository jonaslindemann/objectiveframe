#ifndef _CFemPipeSection_H_
#define _CFemPipeSection_H_

#include "FemSection.h"

#define FEM_PIPE_SECTION 3

FemSmartPointer(CFemPipeSection);

/**
 * FemPipeSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (Pipe-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemPipeSection : public CFemSection {
public:
    CFemPipeSection();

    /** CFemPipeSection constructor */
    CFemPipeSection (double outerRadius, double innerRadius);

    /** CFemPipeSection destructor */
    virtual ~CFemPipeSection ();

    FemClassInfo("CFemPipeSection",CFemSection);

    /**
    * Get section size
    *
    * This function gives all measurements associated with the Pipe-Section.
    *
    */
    void getSectionSize(double &outerRadius, double &innerRadius);

    /**
    * Set section size
    *
    * This function sets all measurements associated with the Pipe-Section.
    *
    */
    void setSectionSize(double outerRadius, double innerRadius);

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






