#ifndef _CFemISection_H_
#define _CFemISection_H_

#include "FemSection.h"

#define FEM_I_SECTION 1

FemSmartPointer(CFemISection);

/**
 * FemISection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (I-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemISection : public CFemSection {
private:
    double m_Ytp;
    double m_Xtp;
public:
    CFemISection();

    /** CFemISection constructor */
    CFemISection (double height, double UFW, double LFW, double WT,
                  double UFT, double LFT, double ULFW, double LLFW);

    /** CFemISection destructor */
    virtual ~CFemISection ();

    FemClassInfo("CFemISection",CFemSection);

    /**
    * Get section size
    *
    * This function gives all measurements associated with the I-Section.
    *
    */
    void getSectionSize(double &height, double &UFW, double &LFW, double &WT,
                        double &UFT, double &LFT, double &ULFW, double &LLFW);

    /**
    * Set section size
    *
    * This function sets all measurements associated with the I-Section.
    *
    */
    void setSectionSize(double height, double UFW, double LFW, double WT,
                        double UFT, double LFT, double ULFW, double LLFW);

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
