#pragma once

#include <memory>
#include <string>

#include <ivf/Camera.h>
#include <ivf/Workspace.h>

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class ViewWindow : public UiWindow {
private:
    // MSAA render target
    GLuint m_msaaFbo{0};
    GLuint m_msaaColorRbo{0};
    GLuint m_msaaDepthRbo{0};

    // Resolve target (texture displayed by ImGui::Image)
    GLuint m_fbo{0};
    GLuint m_colorTexture{0};

    int m_fboWidth{0};
    int m_fboHeight{0};
    int m_sampleCount{4};

    ivf::CameraPtr m_camera;
    ivf::Workspace* m_scene{nullptr};
    FemViewWindow *m_sourceView{nullptr};
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
    void setSourceView(FemViewWindow *view);
    void setWorkspaceSize(double size);
    void setSyncCamera(bool sync);
    bool syncCamera() const;
};

typedef std::shared_ptr<ViewWindow> ViewWindowPtr;

} // namespace ofui
