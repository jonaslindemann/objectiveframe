#pragma once

#include <ofem/section.h>

#define FEM_RECT_SECTION 4

namespace ofem
{

SmartPointer(RectSection);

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
class RectSection : public Section
{
private:
public:
    RectSection();

    /** CFemRectSection constructor */
    RectSection(double width, double height);

    /** CFemRectSection destructor */
    virtual ~RectSection();

    ClassInfo("RectSection", Section);

    /**
     * Get section size
     *
     * This function gives all measurements associated with the Rect-Section.
     *
     */
    void getSectionSize(double& width, double& height);

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

    virtual void getExcZ(double& emax, double& emin) override;
    virtual void getExcY(double& emax, double& emin) override;

    virtual void calcDataFromSection() override;

    virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT,
        double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius) override;

    virtual void getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT,
        double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius) override;
};
}
