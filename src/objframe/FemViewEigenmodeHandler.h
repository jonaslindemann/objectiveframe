#pragma once

class FemViewWindow;

class FemViewEigenmodeHandler {
public:
    static void compute(FemViewWindow &view, int numModes);
    static void clear(FemViewWindow &view);
    static void setVisualization(FemViewWindow &view, int mode);
    static void updateVisualization(FemViewWindow &view, float phase);
    static void setInSecondaryView(FemViewWindow &view, bool flag);
    static void applyAnimation(FemViewWindow &view);
    static void clearAnimation(FemViewWindow &view);
};
