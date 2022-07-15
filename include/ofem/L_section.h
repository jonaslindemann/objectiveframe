#pragma once

#include <ofem/section.h>

#define FEM_L_SECTION 2

namespace ofem {

    SmartPointer(LSection);

    /**
     * FemLSection class.
     *
     * This class is an specialisation of the generic class FemSection.
     * It uses variables inherited from FemSection to represent a specific
     * type of crossection (L-Section). This class stores all measurements and
     * calculates the properties needed to make FEM-calculation (A, Iy, Iz, Kv)
     *
     * @author Pierre Olsson
     * @version 1.0.0
     */
    class LSection : public Section {
    private:
        double m_Xtp;
        double m_Ytp;
    public:
        LSection();

        /** CFemLSection constructor */
        LSection(double height, double width, double WT, double LFT);

        /** CFemLSection destructor */
        virtual ~LSection();

        ClassInfo("LSection", Section);

        /**
        * Get section size
        *
        * This function gives all measurements associated with the L-Section.
        *
        */
        void getSectionSize(double& width, double& height, double& WT, double& LFT);

        /**
        * Set section size
        *
        * This function sets all measurements associated with the L-Section.
        *
        */
        void setSectionSize(double width, double height, double WT, double LFT);

        /**
        * Set data
        *
        * This function (re-) calculates and sets section properties (A, Iy, Iz, Kv).
        *
        */
        void setData();

        virtual void calcDataFromSection();

        virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT,
            double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius) override;

        virtual void getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT,
            double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius) override;

    };
}
