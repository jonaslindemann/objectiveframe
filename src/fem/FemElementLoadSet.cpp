#include "FemElementLoadSet.h"

#include "FemElementLoad.h"

CFemElementLoadSet::CFemElementLoadSet()
:CFemLoadSet()
{

}

CFemElementLoadSet::~CFemElementLoadSet()
{

}

CFemLoad* CFemElementLoadSet::createLoad()
{
	return new CFemElementLoad();
}
