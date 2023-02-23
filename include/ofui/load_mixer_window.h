#pragma once

#include <ofui/ui_window.h>

#include <ofem/beam_node_load_set.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif 

namespace ofui
{

class LoadMixerWindow : public UiWindow
{
private:
    ofem::BeamNodeLoadSet* m_femNodeLoadSet;
    float m_loadScaleFactors[255];
    float m_loadSum;
    float m_lastSum;


#ifdef USE_FEMVIEW
    FemViewWindow* m_view;
#else
    FemWidget* m_view;
#endif

public:
    LoadMixerWindow(const std::string name);
    virtual ~LoadMixerWindow();

    void setView(FemViewWindow* view);
    void setFemNodeLoadSet(ofem::BeamNodeLoadSet* bcSet);

    void update();

    static std::shared_ptr<LoadMixerWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<LoadMixerWindow> LoadMixerWindowPtr;

}
