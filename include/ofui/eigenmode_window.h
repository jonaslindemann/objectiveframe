#pragma once

#include <ofui/ui_window.h>
#include <string>
#include <vector>

class FemViewWindow;

namespace ofui {

class EigenmodeWindow : public UiWindow {
private:
    int m_numModesToCompute;
    int m_currentMode;
    bool m_hasEigenmodes;
    bool m_animate;
    float m_animationSpeed;
    float m_animationPhase;
    double m_modeScaleFactor;
    float m_sliderMax{10.0f};
    std::vector<double> m_eigenvalues;

    FemViewWindow* m_femView;
    
    void onComputeButtonClicked();
    void onClearButtonClicked();
    void onModeChanged(int mode);
    
public:
    EigenmodeWindow(const std::string& title);
    virtual ~EigenmodeWindow() = default;
    
    static std::shared_ptr<EigenmodeWindow> create(const std::string& title);
    
    virtual void doDraw() override;

    /**
     * Sets the title from the interface profile, before UiWindow::draw() opens
     * the window with it. A simple profile reduces this panel to "is the
     * structure stable, and if not how does it fold", which is not what
     * "Eigenmode Analysis" says to someone who has not met the word.
     */
    virtual void doPreDraw() override;
    
    void setFemView(::FemViewWindow* view);
    void setHasEigenmodes(bool hasEigenmodes);
    bool hasEigenmodes() const;
    void setEigenvalues(const std::vector<double>& eigenvalues);
    void setNumModes(int numModes);
    void setCurrentMode(int mode);
    
    int getCurrentMode() const;
    void setAnimate(bool animate);
    bool isAnimate() const;
    float getAnimationPhase() const;
    double getModeScaleFactor() const;
    void setModeScaleFactor(double factor);
    void setScaleSliderMax(float maxVal);
    
    void updateAnimationPhase(float deltaTime);
};

typedef std::shared_ptr<EigenmodeWindow> EigenmodeWindowPtr;

}
