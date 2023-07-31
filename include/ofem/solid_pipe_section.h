#pragma once

#include <ofem/section.h>

#define FEM_SOLIDPIPE_SECTION 6

namespace ofem {

SmartPointer(SolidPipeSection);

/**
 * FemSolidPipeSection class.
 *
 * This class is an specialisation of the generic class FemSection.
 * It uses variables inherited from FemSection to represent a specific
 * type of crossection (SolidPipe-Section). This class stores all measurements and
 * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
 *
 * @author Pierre Olsson
 * @version 1.0.0
 */
class SolidPipeSection : public Section {
public:
    SolidPipeSection();

    /** CFemSolidPipeSection constructor */
    SolidPipeSection(double outerRadius);

    /** CFemSolidPipeSection destructor */
    virtual ~SolidPipeSection();

    ClassInfo("SolidPipeSection", Section);
    StdFactory(SolidPipeSection);

    /**
     * Get section size
     *
     * This function gives all measurements associated with the SolidPipe-Section.
     *
     */
    void getSectionSize(double &outerRadius);

    /**
     * Set section size
     *
     * This function sets all measurements associated with the SolidPipe-Section.
     *
     */
    void setSectionSize(double outerRadius);

    /**
     * Set data
     *
     * This function (re-) calculates and sets section properties (A, Iy, Iz, Kv).
     *
     */
    void setData();

    virtual void getExcZ(double &emax, double &emin) override;
    virtual void getExcY(double &emax, double &emin) override;

    virtual void calcDataFromSection() override;

    virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT, double UFT, double LFT,
                                 double ULFW, double LLFW, double outerRadius, double innerRadius) override;

    virtual void getSectionProps(double &width, double &height, double &UFW, double &LFW, double &WT, double &UFT,
                                 double &LFT, double &ULFW, double &LLFW, double &outerRadius,
                                 double &innerRadius) override;
};
} // namespace ofem
