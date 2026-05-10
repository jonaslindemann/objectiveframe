#pragma once

class FemViewWindow;

class FemViewSolverHandler {
public:
    static void executeCalc(FemViewWindow &view);
    static void recompute(FemViewWindow &view);
};
