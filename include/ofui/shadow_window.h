#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

/**
 * Shadow settings panel.
 *
 * The light that casts shadows is deliberately not the scene light -- that one
 * is fixed to the camera, and a shadow that followed it would swing around the
 * model as it was orbited. This panel is where the shadow light is aimed
 * instead, as a compass bearing and a height above the horizon.
 */
class ShadowWindow : public UiWindow {
private:
    bool m_useShadows;
    float m_azimuth;
    float m_elevation;
    float m_strength;
    int m_mapSizeIndex;

    FemViewWindow *m_view;

    /** Pull the current values out of the view. */
    void update();

public:
    ShadowWindow(const std::string name);
    virtual ~ShadowWindow();

    void setView(FemViewWindow *view);

    static std::shared_ptr<ShadowWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ShadowWindow> ShadowWindowPtr;

} // namespace ofui
