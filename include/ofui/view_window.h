#pragma once

#include <memory>
#include <string>

#include <ivf/Camera.h>
#include <ivf/Workspace.h>

#include <ofui/ui_window.h>

class IvfViewWindow;

namespace ofui {

class ViewWindow : public UiWindow {
private:
    GLuint m_fbo{0};
    GLuint m_colorTexture{0};
    GLuint m_depthRbo{0};
    int m_fboWidth{0};
    int m_fboHeight{0};

    ivf::CameraPtr m_camera;
    ivf::Workspace* m_scene{nullptr};
    IvfViewWindow* m_sourceView{nullptr};
    bool m_syncCamera{true};
    double m_workspaceSize{10.0};

    void initFbo(int w, int h);
    void cleanupFbo();
    void syncCameraFromSource();

protected:
    virtual void doDraw() override;
    virtual void onRenderScene();

public:
    ViewWindow(const std::string& name);
    virtual ~ViewWindow();

    static std::shared_ptr<ViewWindow> create(const std::string& name);

    void setScene(ivf::Workspace* scene);
    void setSourceView(IvfViewWindow* view);
    void setWorkspaceSize(double size);
    void setSyncCamera(bool sync);
    bool syncCamera() const;
};

typedef std::shared_ptr<ViewWindow> ViewWindowPtr;

} // namespace ofui
