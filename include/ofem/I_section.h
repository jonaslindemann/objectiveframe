#pragma once

#include <ofem/section.h>

#define FEM_I_SECTION 1

namespace ofem {

    SmartPointer(ISection);

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
    class ISection : public Section {
    private:
        double m_Ytp;
        double m_Xtp;
    public:
        ISection();

        /** CFemISection constructor */
        ISection(double height, double UFW, double LFW, double WT,
            double UFT, double LFT, double ULFW, double LLFW);

        /** CFemISection destructor */
        virtual ~ISection();

        ClassInfo("ISection", Section);

        /**
        * Get section size
        *
        * This function gives all measurements associated with the I-Section.
        *
        */
        void getSectionSize(double& height, double& UFW, double& LFW, double& WT,
            double& UFT, double& LFT, double& ULFW, double& LLFW);

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

        virtual void getExcZ(double& emax, double& emin);
        virtual void getExcY(double& emax, double& emin);

        virtual void calcDataFromSection();

        virtual void setSectionProps(double width, double height, double UFW, double LFW, double WT,
            double UFT, double LFT, double ULFW, double LLFW, double outerRadius, double innerRadius) override;

        virtual void getSectionProps(double& width, double& height, double& UFW, double& LFW, double& WT,
            double& UFT, double& LFT, double& ULFW, double& LLFW, double& outerRadius, double& innerRadius) override;

    };
}
