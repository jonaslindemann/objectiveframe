#include <iostream>

using namespace std;

#include <FemNode.h>
#include <FemBeam.h>
#include <FemBeamModel.h>
#include <FemBeamLoad.h>
#include <FemBeamNodeLoad.h>
#include <FemBeamNodeBC.h>

#include "BeamSolver.h"


int main(int argc, char **argv)
{
	cout << "Creating beam model..." << endl;
	CFemBeamModelPtr beamModel = new CFemBeamModel();

	cout << "Intialise beam model..." << endl;
	beamModel->setFileName("large_building.df3");

	cout << "Load beam model..." << endl;
	beamModel->open();

	cout << beamModel->nodeSet()->getSize() << endl;
}
