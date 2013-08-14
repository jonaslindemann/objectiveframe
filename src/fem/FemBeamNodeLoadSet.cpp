#include "FemBeamNodeLoadSet.h"

// ------------------------------------------------------------
CFemBeamNodeLoadSet::CFemBeamNodeLoadSet()
:CFemNodeLoadSet()
{

}

// ------------------------------------------------------------
CFemBeamNodeLoadSet::~CFemBeamNodeLoadSet()
{

}

// ------------------------------------------------------------
CFemLoad* CFemBeamNodeLoadSet::createLoad()
{
	return new CFemBeamNodeLoad();
}

