#pragma once

#include <ofem/section.h>

#define FEM_PIPE_SECTION 3

namespace ofem {

SmartPointer(PipeSection);

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
class PipeSection : public Section {
public:
    PipeSection();

    /** CFemPipeSection constructor */
    PipeSection(double outerRadius, double innerRadius);

    /** CFemPipeSection destructor */
    virtual ~PipeSection();

    ClassInfo("PipeSection", Section);
    StdFactory(PipeSection);

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
