#pragma once

#include <ofui/ui_window.h>

#include <ofem/beam_node_load_set.h>

class FemViewWindow;

namespace ofui {

class LoadMixerWindow : public UiWindow {
private:
    ofem::BeamNodeLoadSet *m_femNodeLoadSet;
    float m_loadScaleFactors[255];
    float m_loadSum;
    float m_lastSum;
    bool m_firstUpdate;

    FemViewWindow *m_view;

public:
    LoadMixerWindow(const std::string name);
    virtual ~LoadMixerWindow();

    void setView(FemViewWindow *view);
    void setFemNodeLoadSet(ofem::BeamNodeLoadSet *bcSet);

    void update();

    static std::shared_ptr<LoadMixerWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<LoadMixerWindow> LoadMixerWindowPtr;

} // namespace ofui
