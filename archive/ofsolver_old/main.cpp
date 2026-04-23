#include <iostream>

using namespace std;

#include <FemNode.h>
#include <FemBeam.h>
#include <FemBeamModel.h>
#include <FemBeamLoad.h>
#include <FemBeamNodeLoad.h>
#include <FemBeamNodeBC.h>

#include "BeamSolver.h"

#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string currentDir()
{
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		cout << "Failed to get current dir." << endl;
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

	return cCurrentPath;
}


int main(int argc, char **argv)
{
	cout << "Current dir : " << currentDir() << endl;

	cout << "Creating beam model..." << endl;
	FemBeamModelPtr beamModel = new BeamModel();

	cout << "Intialise beam model..." << endl;
	beamModel->setFileName("large_building.df3");

	cout << "Load beam model..." << endl;

	if (!beamModel->open())
	{
		cout << "Couldn't open input file." << endl;
		return -1;
	}

	cout << "Create solver." << endl;

	CBeamSolver* solver = new CBeamSolver();
	solver->setBeamModel(beamModel);

	cout << "Execute solver." << endl;
	solver->execute();
}
