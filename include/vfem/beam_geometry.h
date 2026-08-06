#pragma once

//
// Selects which Ivf++ extrusion implementation the beam geometry is built on.
//
// The default is the original gle backed pair, ivf::Extrusion and
// ivf::SolidLine. Configuring with -DOF_SWEPT_EXTRUSION=ON switches to
// ivf::SweptExtrusion and ivf::SweptSolidLine, which generate the swept mesh
// themselves and draw it from a vertex buffer instead of going through gle and
// immediate mode.
//
// The two have the same interface and, in the configurations this application
// uses, produce the same image -- so the switch is a build option rather than
// a rewrite, and can be flipped back if anything looks wrong.
//

#ifdef OF_SWEPT_EXTRUSION

#include <ivf/SweptExtrusion.h>
#include <ivf/SweptSolidLine.h>

namespace vfem {

using BeamExtrusion = ivf::SweptExtrusion;
using BeamExtrusionPtr = ivf::SweptExtrusionPtr;

using BeamSolidLine = ivf::SweptSolidLine;
using BeamSolidLinePtr = ivf::SweptSolidLinePtr;

// Reported by ivf::RTTIBase::isClass(). Used where the scene graph is walked
// looking for solid lines.

constexpr const char *BeamSolidLineClassName = "SweptSolidLine";

} // namespace vfem

#else

#include <ivf/Extrusion.h>
#include <ivf/SolidLine.h>

namespace vfem {

using BeamExtrusion = ivf::Extrusion;
using BeamExtrusionPtr = ivf::ExtrusionPtr;

using BeamSolidLine = ivf::SolidLine;
using BeamSolidLinePtr = ivf::SolidLinePtr;

constexpr const char *BeamSolidLineClassName = "SolidLine";

} // namespace vfem

#endif
