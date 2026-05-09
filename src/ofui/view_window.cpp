#include <ofui/view_window.h>

#include <algorithm>

#include <imgui.h>
#include <ivf/Camera.h>
#include <ivf/Workspace.h>

#include <FemView.h>

#include <vfem/Preferences.h>

using namespace ofui;

ViewWindow::ViewWindow(const std::string& name)
    : UiWindow(name)
{
    m_camera = ivf::Camera::create();
    m_camera->setPerspective(45.0, 0.1, 100.0);

    setWindowFlags(ImGuiWindowFlags_None);
}

ViewWindow::~ViewWindow()
{
    cleanupFbo();
}

std::shared_ptr<ViewWindow> ViewWindow::create(const std::string& name)
{
    return std::make_shared<ViewWindow>(name);
}

void ViewWindow::setScene(ivf::Workspace* scene)
{
    m_scene = scene;
}

void ViewWindow::setSourceView(FemViewWindow *view)
{
    m_sourceView = view;
}

void ViewWindow::setWorkspaceSize(double size)
{
    m_workspaceSize = size;
}

void ViewWindow::setSyncCamera(bool sync)
{
    m_syncCamera = sync;
}

bool ViewWindow::syncCamera() const
{
    return m_syncCamera;
}

void ViewWindow::initFbo(int w, int h)
{
    cleanupFbo();

    // MSAA render target
    glGenRenderbuffers(1, &m_msaaColorRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaColorRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sampleCount, GL_RGBA8, w, h);

    glGenRenderbuffers(1, &m_msaaDepthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_msaaDepthRbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_sampleCount, GL_DEPTH_COMPONENT24, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_msaaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_msaaColorRbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_msaaDepthRbo);

    // Resolve target — regular 2D texture for ImGui::Image
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_fboWidth = w;
    m_fboHeight = h;
}

void ViewWindow::cleanupFbo()
{
    if (m_msaaFbo) { glDeleteFramebuffers(1, &m_msaaFbo); m_msaaFbo = 0; }
    if (m_msaaColorRbo) { glDeleteRenderbuffers(1, &m_msaaColorRbo); m_msaaColorRbo = 0; }
    if (m_msaaDepthRbo) { glDeleteRenderbuffers(1, &m_msaaDepthRbo); m_msaaDepthRbo = 0; }
    if (m_fbo) { glDeleteFramebuffers(1, &m_fbo); m_fbo = 0; }
    if (m_colorTexture) { glDeleteTextures(1, &m_colorTexture); m_colorTexture = 0; }
    m_fboWidth = 0;
    m_fboHeight = 0;
}

void ViewWindow::syncCameraFromSource()
{
    if (!m_sourceView)
        return;

    ivf::Camera* src = m_sourceView->getCamera();

    double x, y, z;
    src->getPosition(x, y, z);
    m_camera->setPosition(x, y, z);

    src->getTarget(x, y, z);
    m_camera->setTarget(x, y, z);

    double fov, zNear, zFar;
    src->getPerspective(fov, zNear, zFar);
    m_camera->setPerspective(fov, zNear, zFar);
}

void ViewWindow::onRenderScene()
{
    if (!m_scene)
        return;

    auto showNodeNumbers = vfem::Preferences::instance().showNodeNumbers();
    vfem::Preferences::instance().setShowNodeNumbers(false);
    m_sourceView->showTextLayer(false);

    bool secondaryEigenmode = m_sourceView != nullptr && m_sourceView->shouldAnimateInSecondaryView();
    if (secondaryEigenmode)
        m_sourceView->applyEigenmodeAnimation();

    m_scene->render();

    if (secondaryEigenmode)
        m_sourceView->clearEigenmodeAnimation();

    m_sourceView->showTextLayer(true);
    vfem::Preferences::instance().setShowNodeNumbers(showNodeNumbers);
}

void ViewWindow::doDraw()
{
    if (!m_scene)
    {
        ImGui::TextDisabled("No scene assigned.");
        return;
    }

    // Sync toggle button in top bar
    if (m_syncCamera)
    {
        if (ImGui::SmallButton("Unlock"))
            m_syncCamera = false;
        ImGui::SameLine();
        ImGui::TextDisabled("Synced with main view");
    }
    else
    {
        if (ImGui::SmallButton("Sync"))
            m_syncCamera = true;
        ImGui::SameLine();
        ImGui::TextDisabled("Independent view");
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    int w = std::max(1, (int)avail.x);
    int h = std::max(1, (int)avail.y);

    if (w != m_fboWidth || h != m_fboHeight)
        initFbo(w, h);

    if (m_syncCamera)
        syncCameraFromSource();

    // Save framebuffer binding (not covered by glPushAttrib)
    GLint prevFbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);

    // Save all fixed-function attribute state: enables (GL_BLEND, GL_DEPTH_TEST,
    // GL_LIGHTING, GL_TEXTURE_1D/2D, GL_SCISSOR_TEST, GL_TEXTURE_GEN_S/T, ...),
    // viewport, blend func, lighting, texture bindings/env, color material, etc.
    // Result-mode rendering leaves GL_TEXTURE_1D enabled, sets the GL_TEXTURE matrix
    // via Texture::apply(), and may change other state — save everything at once.
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Save matrix stacks (glPushAttrib does not save actual matrix values)
    GLint prevMatrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &prevMatrixMode);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Swap scene camera to ours, render into FBO, restore
    ivf::View* prevView = m_scene->getView();

    m_camera->setViewPort(w, h);
    m_camera->initialize();
    m_scene->setView(m_camera);

    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

    // Render into MSAA FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_msaaFbo);
    glViewport(0, 0, w, h);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    onRenderScene();

    // Resolve MSAA → regular texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaaFbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Restore framebuffer first, then matrices, then attributes (glPopAttrib restores viewport)
    glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(prevMatrixMode);

    glPopAttrib();

    m_scene->setView(prevView);

    // OpenGL FBO origin is bottom-left; ImGui expects top-left — flip UV vertically
    ImGui::Image((ImTextureID)(intptr_t)m_colorTexture, avail, ImVec2(0, 1), ImVec2(1, 0));

    // Independent camera mouse interaction
    if (!m_syncCamera && ImGui::IsItemHovered())
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.MouseDown[0])
        {
            m_camera->rotatePositionY(io.MouseDelta.x / 100.0);
            m_camera->rotatePositionX(io.MouseDelta.y / 100.0);
        }
        if (io.MouseDown[1])
        {
            m_camera->moveSideways(-io.MouseDelta.x * m_workspaceSize / 1000.0);
            m_camera->moveVertical(io.MouseDelta.y * m_workspaceSize / 1000.0);
        }
        if (io.MouseWheel != 0.0f)
            m_camera->moveDepth(-io.MouseWheel * m_workspaceSize / 10.0);
    }
}
