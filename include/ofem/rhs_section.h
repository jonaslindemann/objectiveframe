#pragma once

#include <ofem/section.h>

#define FEM_RHS_SECTION 5

namespace ofem
{

SmartPointer(RHSSection);

/**
 * FemRHSSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (RHS-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class RHSSection : public Section
{
public:
    RHSSection();

    /** CFemRHSSection constructor */
    RHSSection(double height, double width, double WT);

    /** CFemRHSSection destructor */
    virtual ~RHSSection();

    ClassInfo("RHSSection", Section);

    /**
     * Get section size
     *
     * This function gives all measurements associated with the RHS-Section.
     *
     */
    void getSectionSize(double& height, double& width, double& WT);

    /**
     * Set section size
     *
     * This function sets all measurements associated with the RHS-Section.
     *
     */
    void setSectionSize(double height, double width, double WT);

    /**
     * Set data
     *
     * This function (re-) calculates and sets section properties (A, Iy, Iz, Kv).
     *
     */
    void setData();

    virtual void getExcZ(double& emax, double& emin) override;
    virtual void getExcY(double& emax, double& emin) override;

    virtual void calcDataFromSection() override;

    virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT,
        double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius) override;

    virtual void getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT,
        double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius) override;
};
}
