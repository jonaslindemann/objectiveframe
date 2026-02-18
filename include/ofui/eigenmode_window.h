#pragma once

#include <ofui/ui_window.h>
#include <string>

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
    
    FemViewWindow* m_femView;
    
    void onComputeButtonClicked();
    void onClearButtonClicked();
    void onModeChanged(int mode);
    
public:
    EigenmodeWindow(const std::string& title);
    virtual ~EigenmodeWindow() = default;
    
    static std::shared_ptr<EigenmodeWindow> create(const std::string& title);
    
    virtual void doDraw() override;
    
    void setFemView(::FemViewWindow* view);
    void setHasEigenmodes(bool hasEigenmodes);
    void setNumModes(int numModes);
    
    int getCurrentMode() const;
    bool isAnimate() const;
    float getAnimationPhase() const;
    double getModeScaleFactor() const;
    void setModeScaleFactor(double factor);
    
    void updateAnimationPhase(float deltaTime);
};

typedef std::shared_ptr<EigenmodeWindow> EigenmodeWindowPtr;

}
