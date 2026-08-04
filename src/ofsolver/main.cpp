
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <ofem/beam_model.h>
#include <ofsolve/beam_solver.h>

using namespace calfem;
using namespace ofem;
using namespace ofsolver;

namespace {

double percentile(std::vector<double> values, double p)
{
    if (values.empty())
        return 0.0;

    std::sort(values.begin(), values.end());
    const double pos = std::clamp(p, 0.0, 1.0) * static_cast<double>(values.size() - 1);
    const size_t lower = static_cast<size_t>(std::floor(pos));
    const size_t upper = std::min(lower + 1, values.size() - 1);
    const double weight = pos - static_cast<double>(lower);

    return values[lower] * (1.0 - weight) + values[upper] * weight;
}

void printResultSummary(BeamModel *beamModel)
{
    std::vector<double> nAbs;
    std::vector<double> mAbs;
    std::vector<double> mNorm;
    int beams = 0;
    int bars = 0;

    auto elementSet = beamModel->getElementSet();

    for (auto i = 0; i < elementSet->getSize(); i++) {
        auto beam = static_cast<Beam *>(elementSet->getElement(i));
        if (beam->beamType() == btBeam)
            beams++;
        else
            bars++;

        const int n = beam->getEvaluationPoints();
        for (int k = 0; k < n; k++) {
            const double N = beam->getValue(0 + 6 * k);
            const double My = beam->getValue(4 + 6 * k);
            const double Mz = beam->getValue(5 + 6 * k);
            const double M = std::sqrt(My * My + Mz * Mz);

            if (std::isfinite(N))
                nAbs.push_back(std::fabs(N));
            if (std::isfinite(M)) {
                mAbs.push_back(M);
                if (beamModel->maxM() > 0.0)
                    mNorm.push_back(M / beamModel->maxM());
            }
        }
    }

    std::cout << "Elements: beams=" << beams << " bars=" << bars << "\n";
    std::cout << "Display max N=" << beamModel->maxN()
              << " min N=" << beamModel->minN()
              << " max M=" << beamModel->maxM() << "\n";
    std::cout << "Abs N p50/p75/p90/p98/max = "
              << percentile(nAbs, 0.50) << " / "
              << percentile(nAbs, 0.75) << " / "
              << percentile(nAbs, 0.90) << " / "
              << percentile(nAbs, 0.98) << " / "
              << percentile(nAbs, 1.00) << "\n";
    std::cout << "Abs M p50/p75/p90/p98/max = "
              << percentile(mAbs, 0.50) << " / "
              << percentile(mAbs, 0.75) << " / "
              << percentile(mAbs, 0.90) << " / "
              << percentile(mAbs, 0.98) << " / "
              << percentile(mAbs, 1.00) << "\n";
    std::cout << "Norm M p50/p75/p90/p98/max = "
              << percentile(mNorm, 0.50) << " / "
              << percentile(mNorm, 0.75) << " / "
              << percentile(mNorm, 0.90) << " / "
              << percentile(mNorm, 0.98) << " / "
              << percentile(mNorm, 1.00) << "\n";
}

} // namespace

int main(int argc, char **argv)
{
    BeamModelPtr beamModel = BeamModel::create();
    beamModel->setFileName(argc > 1 ? argv[1] : "large_building.df3");
    if (!beamModel->open())
    {
        std::cout << "Couldn't open model. Check filename." << "\n";

        return -1;
    }

    std::cout << "Beam model loaded."
              << "\n";

    BeamSolverPtr solver = BeamSolver::create();
    solver->setBeamModel(beamModel);

    bool runUpdateCycle = false;
    int feedbackArg = 2;

    if (argc > 2 && std::string(argv[2]) == "--update-cycle") {
        runUpdateCycle = true;
        feedbackArg = 3;
    }

    if (argc == feedbackArg + 4) {
        const int nodeNumber = std::atoi(argv[feedbackArg]);
        const double fx = std::atof(argv[feedbackArg + 1]);
        const double fy = std::atof(argv[feedbackArg + 2]);
        const double fz = std::atof(argv[feedbackArg + 3]);

        if (nodeNumber > 0 && nodeNumber <= beamModel->getNodeSet()->getSize())
            solver->setFeedbackForce(beamModel->getNodeSet()->getNode(nodeNumber - 1), fx, fy, fz);
        else
            std::cout << "Ignoring invalid feedback node " << nodeNumber << "\n";
    }

    solver->execute();
    std::cout << "After execute:\n";
    printResultSummary(beamModel.get());

    if (runUpdateCycle) {
        solver->recompute();
        solver->update();
        std::cout << "After recompute/update:\n";
        printResultSummary(beamModel.get());
    }

    return 0;
}
