#ifndef _CFemUSection_H_
#define _CFemUSection_H_

#include "FemSection.h"

#define CFEMUSECTION 10105
#define FEM_U_SECTION 8

FemSmartPointer(CFemUSection);

/**
 * FemUSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (U-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class CFemUSection : public CFemSection {
private:
    double m_Ytp;
    double m_Xtp;
public:
    CFemUSection();

    /** CFemUSection constructor */
    CFemUSection (double height, double UFW, double LFW, double WT, double UFT, double LFT);

    /** CFemUSection destructor */
    virtual ~CFemUSection ();

    FemClassInfo("CFemUSection",CFemSection);

    /**
    * Get section size
    *
    * This function gives all measurements associated with the U-Section.
    *
    */
    void getSectionSize(double &height, double &UFW, double &LFW, double &WT, double &UFT, double &LFT);

    /**
    * Set section size
    *
    * This function sets all measurements associated with the U-Section.
    *
    */
    void setSectionSize(double height, double UFW, double LFW, double WT, double UFT, double LFT);

    /**
    * Set data
    *
    * This function (re-) calculates and sets section properties (A, Iy, Iz, Kv).
    *
    */
    void setData();

    virtual void getExcZ(double &emax, double &emin);
    virtual void getExcY(double &emax, double &emin);

    virtual void calcDataFromSection() override;

    virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT,
        double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius);

    virtual void getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT,
        double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius);

};
#endif






