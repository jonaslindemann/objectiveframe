
#include <Eigen/Dense>
#include <iomanip>
#include <iostream>
#include <vector>

#include <ofem/beam_model.h>
#include <ofsolve/beam_solver.h>

using namespace calfem;
using namespace ofem;
using namespace ofsolver;

int main()
{
    BeamModelPtr beamModel = BeamModel::create();
    beamModel->setFileName("large_building.df3");
    if (!beamModel->open())
    {
        std::cout << "Couldn't open model. Check filename." << "\n";

        return -1;
    }

    std::cout << "Beam model loaded."
              << "\n";

    BeamSolverPtr solver = BeamSolver::create();
    solver->setBeamModel(beamModel);
    solver->execute();

    return 0;
}
