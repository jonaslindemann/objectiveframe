#include "IvfViewWindow.h"

#include <iostream>

// #undef GLAD_GL_IMPLEMENTATION
// #include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>

using namespace std;
using namespace ivf;

std::shared_ptr<IvfViewWindow> IvfViewWindow::create(int width, int height, const std::string title,
                                                     GLFWmonitor *monitor, GLFWwindow *shared)
{
    return std::make_shared<IvfViewWindow>(width, height, title, monitor, shared);
}

IvfViewWindow::IvfViewWindow(int width, int height, const std::string title, GLFWmonitor *monitor, GLFWwindow *shared)
    : GLFWWindow(width, height, title, monitor, shared), m_currentButton{ButtonState::bsNoButton},
      m_currentModifier{ButtonState::bsNoButton}, m_angleX{0.0f}, m_angleY{0.0f}, m_moveX{0.0f}, m_moveY{0.0f},
      m_zoomX{0.0f}, m_zoomY{0.0f}, m_snapToGrid{true}, m_selectedShape{nullptr}, m_editMode{WidgetMode::ViewPan},
      m_clickNumber{0}, m_nNodes{0}, m_nLines{0}, m_doOverlay{false}, m_doUnderlay{false}, m_editEnabled{true},
      m_selectEnabled{true}, m_lastShape{nullptr}, m_initDone{false}, m_mouseUpdate{false},
      m_workspaceSize{10.0f}, m_quit{false}, m_customPick{false}, m_lockSceneRendering{false}
{
    // Create default camera

    m_camera = Camera::create();
    m_camera->setPosition(0.0, m_workspaceSize / 8.0, -m_workspaceSize / 2.0);
    m_camera->setPerspective(45.0, 0.1, 100.0);

    // Create scene

    m_scene = Workspace::create();
    m_scene->setView(m_camera);
    m_scene->disableCursor();

    // Create selected shapes list.

    m_selectedShapes = Composite::create();
    m_selectedShapes->setUseReference(false);

    // Create volume selection box;

    m_volumeSelection = WireBrick::create();
    m_volumeSelection->setState(Shape::OS_OFF);

    m_scene->addChild(m_volumeSelection);

    m_xyPlane.setPlaneNormal(0.0, 0.0, 1.0);
    m_yzPlane.setPlaneNormal(1.0, 0.0, 0.0);

    this->setEditMode(WidgetMode::CreateNode);
}

IvfViewWindow::~IvfViewWindow()
{
    onDestroy();
}

void IvfViewWindow::onGlfwKey(int key, int scancode, int action, int mods)
{
    m_currentModifier = ButtonState::bsNoButton;

    if (isShiftDown())
        m_currentModifier = ButtonState::bsShift;
    if (isCtrlDown())
        m_currentModifier = ButtonState::bsCtrl;
    if (isAltDown())
        m_currentModifier = ButtonState::bsAlt;

    if ((key < GLFW_KEY_LEFT_SHIFT) && (m_currentModifier != ButtonState::bsNoButton) && (action == GLFW_PRESS)) {
        if (m_currentModifier == ButtonState::bsShift)
            doShortcut(ModifierKey::mkShift, key);
        if (m_currentModifier == ButtonState::bsCtrl)
            doShortcut(ModifierKey::mkCtrl, key);
        if (m_currentModifier == ButtonState::bsAlt)
            doShortcut(ModifierKey::mkAlt, key);
    }
    else {
        if (action == GLFW_PRESS)
            doKeyboard(key);
        else
            doKeyboardReleased(key);
    }
}

void IvfViewWindow::onGlfwMousePosition(double x, double y)
{
    if (isAnyMouseButtonDown()) {
        doMotion(mouseX(), mouseY());
    }
    else
        doPassiveMotion(mouseX(), mouseY());
}

void IvfViewWindow::onGlfwMouseButton(int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_1)
            m_currentButton = ButtonState::bsButton1;

        if (button == GLFW_MOUSE_BUTTON_2)
            m_currentButton = ButtonState::bsButton2;

        if (button == GLFW_MOUSE_BUTTON_3)
            m_currentButton = ButtonState::bsButton3;
        if (!isOverWindow()) {
            doMouseDown(mouseX(), mouseY());
            doMouse(mouseX(), mouseY());
        }
    }
    else {
        if (!isOverWindow())
            doMouseUp(mouseX(), mouseY());
    }
}

void IvfViewWindow::onGlfwResize(int width, int height)
{
    // m_camera->setViewPort(this->width(), this->height());
}

void IvfViewWindow::onGlfwDraw()
{
    if (!m_initDone) {
        onInitContext();

        doInitImGui();

        m_lighting = Lighting::getInstance();
        m_lighting->setTwoSide(true);
        m_lighting->enable();

        auto light = m_lighting->getLight(0);
        light->setLightPosition(0.0f, 0.5f, 1.0f, 0.0f);
        light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
        light->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
        light->enable();

        // Create and bind the FBO

        onInit();
        m_initDone = true;
        m_prevWindowSize[0] = width();
        m_prevWindowSize[1] = height();
    }

    // Set up camera

    m_camera->setViewPort(width(), height());
    m_camera->initialize();

    // Drawing code

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    // m_camera->rotateAbsolute(m_angleX / 100.0, m_ang)
    m_camera->rotatePositionY(m_angleX / 100.0);
    m_camera->rotatePositionX(m_angleY / 100.0);

    m_camera->moveSideways(m_moveX * m_workspaceSize / 1000.0);
    m_camera->moveVertical(m_moveY * m_workspaceSize / 1000.0);
    m_camera->moveDepth(m_zoomY * m_workspaceSize / 500.0);

    m_angleX = 0.0;
    m_angleY = 0.0;
    m_moveX = 0.0;
    m_moveY = 0.0;
    m_zoomY = 0.0;

    glPopMatrix();

    glPushMatrix();

    if (m_doUnderlay) {
        glPushAttrib(GL_ENABLE_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)this->width(), (double)this->height(), 0.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        onUnderlay();

        glPopAttrib();

        m_camera->setViewPort(this->width(), this->height());
        m_camera->initialize();
    }

    glPopMatrix();

    if (!m_lockSceneRendering)
        m_scene->render();

    glPushMatrix();

    if (m_doOverlay) {
        glPushAttrib(GL_ENABLE_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)width(), (double)height(), 0.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        onOverlay();

        glPopAttrib();

        m_camera->setViewPort(width(), height());
        m_camera->initialize();
    }

    glPopMatrix();

    this->doDrawImGui();
}

void IvfViewWindow::deleteAll()
{
    m_scene->deleteAll();
    this->redraw();
    m_selectedShapes->clear();
    m_scene->addChild(m_volumeSelection);
}

void IvfViewWindow::deleteSelected()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    for (i = 0; i < m_selectedShapes->getSize(); i++) {
        auto shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape, doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            shape->setSelect(GLBase::SS_OFF);
    }

    m_selectedShapes->clear();
    redraw();
}

void IvfViewWindow::deleteSelectedKeep()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    vector<ShapePtr> remainingShapes;

    for (i = 0; i < m_selectedShapes->getSize(); i++) {
        auto shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape, doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            remainingShapes.push_back(ShapePtr(shape));
    }

    m_selectedShapes->clear();

    for (i = 0; i < remainingShapes.size(); i++)
        m_selectedShapes->addChild(remainingShapes[i]);

    redraw();
}

void IvfViewWindow::createLine()
{
    if (m_selectedShapes->getSize() == 2) {
        Node *node1 = static_cast<Node *>(m_selectedShapes->getChild(0));
        Node *node2 = static_cast<Node *>(m_selectedShapes->getChild(1));

        if ((node1->isClass("Node")) && (node2->isClass("Node"))) {
            Shape *solidLine = nullptr;
            onCreateLine(node1, node2, solidLine);
            if (solidLine != nullptr) {
                addToScene(solidLine);
            }
        }
    }
}

void IvfViewWindow::addToScene(Shape *shape)
{
    m_scene->addChild(shape);
}

void IvfViewWindow::centerSelected()
{
    if (m_selectedShapes->getSize() == 1) {
        Shape *shape = static_cast<Shape *>(m_selectedShapes->getChild(0));
        double x, y, z;
        shape->getPosition(x, y, z);
        m_camera->setTarget(x, y, z);
        redraw();
    }
}

void IvfViewWindow::resetView()
{
    m_camera->setPosition(0.0, m_workspaceSize / 8.0, m_workspaceSize);
    m_camera->setTarget(0.0, 0.0, 0.0);
    redraw();
}

void IvfViewWindow::clearSelection()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = nullptr;

    // Should onDeSelect be called ???

    // onDeSelect();
    redraw();
}

void IvfViewWindow::selectAll()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    int i;

    Shape *shape;
    auto scene = this->getScene()->getComposite();
    bool select;

    for (i = 0; i < scene->getSize(); i++) {
        shape = scene->getChild(i);
        select = true;
        onSelectFilter(shape, select);
        if (select)
            m_selectedShapes->addChild(shape);
    }

    m_selectedShapes->setSelectChildren(GLBase::SS_ON);
    onSelect(m_selectedShapes);

    redraw();
}

void IvfViewWindow::selectAllBox()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    int i;

    Shape *shape;
    auto scene = this->getScene()->getComposite();
    bool select;

    for (i = 0; i < scene->getSize(); i++) {
        shape = scene->getChild(i);
        select = true;
        onSelectFilter(shape, select);
        if ((select) && (isInsideVolume(shape)))
            m_selectedShapes->addChild(shape);
    }

    m_selectedShapes->setSelectChildren(GLBase::SS_ON);
    onSelect(m_selectedShapes);

    redraw();
}

bool IvfViewWindow::isInsideVolume(ivf::Shape *shape)
{
    double x_min, y_min, z_min;
    double x_max, y_max, z_max;

    if (shape->isClass("Node")) {
        if (m_volumeStart[0] < m_volumeEnd[0]) {
            x_min = m_volumeStart[0];
            x_max = m_volumeEnd[0];
        }
        else {
            x_min = m_volumeEnd[0];
            x_max = m_volumeStart[0];
        }

        if (m_volumeStart[1] < m_volumeEnd[1]) {
            y_min = m_volumeStart[1];
            y_max = m_volumeEnd[1];
        }
        else {
            y_min = m_volumeEnd[1];
            y_max = m_volumeStart[1];
        }

        if (m_volumeStart[2] < m_volumeEnd[2]) {
            z_min = m_volumeStart[2];
            z_max = m_volumeEnd[2];
        }
        else {
            z_min = m_volumeEnd[2];
            z_max = m_volumeStart[2];
        }

        double x, y, z;

        shape->getPosition(x, y, z);

        return (x >= x_min) && (x <= x_max) && (y >= y_min) && (y <= y_max) && (z >= z_min) && (z <= z_max);
    }
    else {
        return this->onInsideVolume(shape);
    }
}

void IvfViewWindow::setUseCustomPick(bool flag)
{
    m_customPick = flag;
}

bool IvfViewWindow::useCustomPick()
{
    return m_customPick;
}

void IvfViewWindow::lockSceneRendering()
{
    m_lockSceneRendering = true;
}

void IvfViewWindow::unlockSceneRendering()
{
    m_lockSceneRendering = false;
}

bool IvfViewWindow::isSceneRenderingLocked()
{
    return m_lockSceneRendering;
}

// Get/set methods

void IvfViewWindow::setEditMode(WidgetMode mode)
{
    m_editMode = mode;

    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    if (getEditMode() == WidgetMode::Select) {
        m_selectedShape = NULL;
        m_scene->disableCursor();
    };

    if (getEditMode() == WidgetMode::CreateLine) {
        clearSelection();
        m_scene->disableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
    }

    if (getEditMode() == WidgetMode::CreateNode) {
        clearSelection();
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
        m_scene->unlockCursor();
    }

    if (getEditMode() == WidgetMode::SelectPosition) {
        clearSelection();
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
        m_scene->unlockCursor();
    }

    if ((getEditMode() == WidgetMode::SelectVolume) || (getEditMode() == WidgetMode::BoxSelection)) {
        clearSelection();
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
        m_scene->unlockCursor();
    }

    if (getEditMode() == WidgetMode::Move) {
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_scene->unlockCursor();
    }

    if (getEditMode() == WidgetMode::ViewPan) {
        m_scene->disableCursor();
    }

    if (getEditMode() == WidgetMode::ViewZoom) {
        m_scene->disableCursor();
    }

    redraw();
}

WidgetMode IvfViewWindow::getEditMode()
{
    return m_editMode;
}

Workspace *IvfViewWindow::getScene()
{
    return m_scene;
}

void IvfViewWindow::quit()
{
    m_quit = true;
    this->close();
}

bool IvfViewWindow::isOverWindow()
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void IvfViewWindow::addSelection(ivf::Shape *shape)
{
    m_selectedShapes->addChild(shape);
    m_selectedShapes->setSelectChildren(GLBase::SS_ON);
    onSelect(m_selectedShapes);

    redraw();
}

bool IvfViewWindow::selectionContains(std::string shapeName)
{
    for (auto i = 0; i < m_selectedShapes->getSize(); i++)
        if (m_selectedShapes->getChild(i)->isClass(shapeName))
            return true;

    return false;
}

int IvfViewWindow::selectionShapeCount(std::string shapeName)
{
    int count = 0;

    for (auto i = 0; i < m_selectedShapes->getSize(); i++)
        if (m_selectedShapes->getChild(i)->isClass(shapeName))
            count++;

    return count;
}

Camera *IvfViewWindow::getCamera()
{
    return m_camera;
}

void IvfViewWindow::updateCursor(int x, int y)
{
    ivf::Vec3d v = m_scene->getCamera()->pickVector(x, y);
    ivf::Vec3d o = m_scene->getCamera()->getPosition();

    if (isShiftDown()) {
        double xx, yy, zz;
        v.getComponents(xx, yy, zz);
        glm::vec3 vv(xx, yy, zz);

        float s1 = abs(glm::dot(vv, m_xyPlane.planeNormal()));
        float s2 = abs(glm::dot(vv, m_yzPlane.planeNormal()));

        ivf::Vec3d pos = m_scene->getCursorPosition();

        glm::vec3 ip;

        if (s1 > s2) {
            m_xyPlane.setPlaneOrigin(pos.getComponents());
            m_xyPlane.setOrigin(o.getComponents());
            ip = m_xyPlane.intersect(v.getComponents());
        }
        else {
            m_yzPlane.setPlaneOrigin(pos.getComponents());
            m_yzPlane.setOrigin(o.getComponents());
            ip = m_yzPlane.intersect(v.getComponents());
        }

        m_scene->updateCursor(ip.x, ip.y, ip.z);
    }
    else {
        m_xzPlane.setOrigin(o.getComponents());
        glm::vec3 ip = m_xzPlane.intersect(v.getComponents());

        m_scene->updateCursor(ip.x, ip.y, ip.z);
    }
}

void IvfViewWindow::setWorkspace(double size, bool resetCamera)
{
    m_workspaceSize = size;

    m_scene->setSize(size);

    if (resetCamera)
        m_camera->setPosition(0.0, m_workspaceSize / 8.0, -m_workspaceSize / 2.0);

    m_controlSize = m_workspaceSize / 50.0;
}

double IvfViewWindow::getWorkspace()
{
    return m_scene->getSize();
}

Composite *IvfViewWindow::getSelectedShapes()
{
    return m_selectedShapes;
}

void IvfViewWindow::setUseOverlay(bool flag)
{
    m_doOverlay = flag;
    this->redraw();
}

void IvfViewWindow::setUseUnderlay(bool flag)
{
    m_doUnderlay = flag;
}

bool IvfViewWindow::useUnderlay()
{
    return m_doUnderlay;
}

bool IvfViewWindow::getUseOverlay()
{
    return m_doOverlay;
}

void IvfViewWindow::setEditEnabled(bool flag)
{
    m_editEnabled = flag;
}

void IvfViewWindow::setSelectEnable(bool flag)
{
    m_selectEnabled = flag;
}

void IvfViewWindow::setSnapToGrid(bool flag)
{
    m_snapToGrid = flag;
    // m_scene->setSnapToGrid(m_snapToGrid);
}

bool IvfViewWindow::getSnapToGrid()
{
    return m_snapToGrid;
}

int IvfViewWindow::getManipulatorMode()
{
    return m_manipulatorMode;
}

ButtonState IvfViewWindow::getCurrentMouseButton()
{
    return m_currentButton;
}

// ------------------------------------------------------------
ButtonState IvfViewWindow::getCurrentModifier()
{
    m_currentModifier = ButtonState::bsNoButton;

    if (isShiftDown())
        m_currentModifier = ButtonState::bsShift;
    if (isCtrlDown())
        m_currentModifier = ButtonState::bsCtrl;
    if (isAltDown())
        m_currentModifier = ButtonState::bsAlt;

    return m_currentModifier;
}

void IvfViewWindow::redraw()
{
    this->m_scene->getComposite()->refresh();
}

void IvfViewWindow::doInitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows

    onInitImGui();

    io.DisplaySize = ImVec2((float)width(), (float)height());

    ImGui_ImplGlfw_InitForOpenGL(this->ref(), true);
    ImGui_ImplOpenGL2_Init();

    m_initialised = true;
}

void IvfViewWindow::doDrawImGui()
{
    ImGuiIO &io = ImGui::GetIO();

    // glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    /*
    if (m_firstDraw)
    {
        m_firstDraw = false;
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        onDrawImGui();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }
    */

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // ImGuiStyle &style = ImGui::GetStyle();
    // style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White with 50% opacity

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    ImGui::End();

    //----------------------------------------------------------------------------------

    /*
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 50));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 50));
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("TOOLBAR", NULL, window_flags);
    ImGui::PopStyleVar();

    ImGui::Button("Toolbar goes here", ImVec2(0, 37));

    ImGui::End();
    */

    //----------------------------------------------------------------------------------

    onDrawImGui();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void IvfViewWindow::onInit()
{
}

void IvfViewWindow::onDestroy()
{
}

void IvfViewWindow::onShortcut(ModifierKey modifier, int key)
{
}

void IvfViewWindow::onCreateNode(double x, double y, double z, ivf::Node *&newNode)
{
}

void IvfViewWindow::onCreateLine(ivf::Node *node1, ivf::Node *node2, ivf::Shape *&newLine)
{
}

void IvfViewWindow::onSelect(ivf::Composite *selectedShapes)
{
}

bool IvfViewWindow::onInsideVolume(ivf::Shape *shape)
{
    return false;
}

void IvfViewWindow::onSelectPosition(double x, double y, double z)
{
}

void IvfViewWindow::onDeSelect()
{
}

void IvfViewWindow::onCoordinate(double x, double y, double z)
{
}

void IvfViewWindow::onDeleteShape(ivf::Shape *shape, bool &doit)
{
}

void IvfViewWindow::onInitContext()
{
}

void IvfViewWindow::onOverlay()
{
}

void IvfViewWindow::onUnderlay()
{
}

void IvfViewWindow::onHighlightShape(ivf::Shape *shape)
{
}

void IvfViewWindow::doMouseUp(int x, int y)
{
    // Call onMouseUp event method

    if (getEditMode() == WidgetMode::Move)
        onMoveCompleted();

    this->getScene()->showCursor();
    onMouseUp(x, y);
}

void IvfViewWindow::doMouseDown(int x, int y)
{
    m_moveStart = true;
    m_beginX = x;
    m_beginY = y;
    onMouseDown(x, y);
}

void IvfViewWindow::doPassiveMotion(int x, int y)
{
    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    ofmath::GridPlane gp;

    // m_scene->updateCursor(x, y);
    this->updateCursor(x, y);

    if ((getEditMode() == WidgetMode::Select) && (m_selectEnabled)) {
        if (m_selectedShape != nullptr) {
            m_selectedShape->setHighlight(Shape::HS_OFF);
        }

        // Do pick

        if (m_customPick)
            m_selectedShape = onPick(x, y);
        else {
            m_scene->pick(x, y);
            m_selectedShape = m_scene->getSelectedShape();
        }

        if (m_selectedShape != NULL) {
            bool highlight = true;

            onHighlightFilter(m_selectedShape, highlight);

            if (highlight) {
                m_selectedShape->setHighlight(Shape::HS_ON);
                onHighlightShape(m_selectedShape);
            }
        }

        redraw();
    }

    if (getEditMode() == WidgetMode::CreateNode) {
        double wx, wy, wz;
        Vec3d pos;
        // m_scene->updateCursor(x, y);
        this->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(wx, wy, wz);
        onCoordinate(wx, wy, wz);
        redraw();
    }

    if (getEditMode() == WidgetMode::Move) {
        double wx, wy, wz;
        // m_scene->updateCursor(x, y);
        this->updateCursor(x, y);
        Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(wx, wy, wz);
        onCoordinate(wx, wy, wz);
        redraw();
    }

    if ((getEditMode() == WidgetMode::CreateLine) && (m_selectEnabled)) {
        bool needInvalidate = false;

        if (m_selectedShape != nullptr) {
            m_selectedShape->setHighlight(Shape::HS_OFF);
            needInvalidate = true;
        }

        if (m_customPick)
            m_selectedShape = onPick(x, y);
        else {
            m_scene->pick(x, y);
            m_selectedShape = m_scene->getSelectedShape();
        }

        if (m_selectedShape != nullptr) {
            bool highlight = true;
            onHighlightFilter(m_selectedShape, highlight);
            if (highlight) {
                m_selectedShape->setHighlight(Shape::HS_ON);
                needInvalidate = true;
            }
        }

        if (needInvalidate)
            redraw();
    }

    if ((getEditMode() == WidgetMode::SelectVolume) || (getEditMode() == WidgetMode::BoxSelection)) {
        // m_scene->updateCursor(x, y);
        this->updateCursor(x, y);
        double vx, vy, vz;
        auto pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        if (m_clickNumber == 1) {
            m_volumeEnd[0] = vx;
            m_volumeEnd[1] = vy;
            m_volumeEnd[2] = vz;

            ivf::Point3d p0, p1;
            p0.setComponents(m_volumeStart);
            p1.setComponents(m_volumeEnd);
            m_volumeSelection->setSize(&p0, &p1);
        }
    }

    // Call onPassiveMotion event method

    onPassiveMotion(x, y);
}

void IvfViewWindow::doMotion(int x, int y)
{
    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    this->getScene()->showCursor();
    // if ( (getEditMode()>=IVF_VIEW) && (getEditMode()<IVF_CREATE) )
    {
        if ((mouseButton() == GLFW_MOUSE_BUTTON_RIGHT) && (mouseAction() == GLFW_PRESS)) {
            // if ((getEditMode()==IVF_VIEW_ZOOM)||(getEditMode()==IVF_VIEW_PAN))
            {
                this->getScene()->hideCursor();
                if (getCurrentModifier() == ButtonState::bsAlt) {
                    m_zoomX = ((float)x - m_beginX);
                    m_zoomY = ((float)y - m_beginY);
                }
                else if (getCurrentModifier() == ButtonState::bsShift) {
                    m_moveX = ((float)x - m_beginX);
                    m_moveY = ((float)y - m_beginY);
                }
                else {
                    m_angleX = ((float)x - m_beginX);
                    m_angleY = ((float)y - m_beginY);
                }
            }
            m_beginX = x;
            m_beginY = y;

            m_scene->updateSizes();
            redraw();
            draw();
        }
    }

    if (getEditMode() == WidgetMode::Move && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT)) {
        // m_scene->updateCursor(x, y);
        this->updateCursor(x, y);
        double x, y, z;
        double dx, dy, dz;
        bool doit = true;
        Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(x, y, z);

        dx = x - m_startPos[0];
        dy = y - m_startPos[1];
        dz = z - m_startPos[2];
        m_startPos[0] = m_startPos[0] + dx;
        m_startPos[1] = m_startPos[1] + dy;
        m_startPos[2] = m_startPos[2] + dz;

        if (m_moveStart) {
            // onMoveStart();
            m_moveStart = false;
        }

        onMove(m_selectedShapes, dx, dy, dz, doit);

        if (doit) {
            for (int i = 0; i < m_selectedShapes->getSize(); i++) {
                auto shape = m_selectedShapes->getChild(i);
                if (shape->isClass("vfem::Node")) {
                    shape->getPosition(x, y, z);
                    shape->setPosition(x + dx, y + dy, z + dz);
                }
            }
            m_scene->getComposite()->refresh();
            redraw();
            draw();
        }
    }

    // Call onMotion event method

    onMotion(x, y);
}

void IvfViewWindow::doMouse(int x, int y)
{
    // Store mouse down position.

    Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
    pos.getComponents(m_startPos[0], m_startPos[1], m_startPos[2]);

    if ((m_editMode == WidgetMode::Select) && (m_selectEnabled) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT)) {
        if (m_selectedShape != NULL) {
            if (m_selectedShape->getSelect() != GLBase::SS_ON) {
                bool select = true;
                onSelectFilter(m_selectedShape, select);
                if (select) {
                    m_selectedShape->setSelect(GLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                    onSelect(m_selectedShapes);
                }
                redraw();
            }
        }
        else {
            m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
            m_selectedShapes->clear();
            onDeSelect();
            redraw();
            draw();
        }
    }

    // Handle node creation

    if ((m_editMode == WidgetMode::CreateNode) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT)) {
        double vx, vy, vz;
        Node *node = NULL;
        Vec3d pos;

        this->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        if (m_editEnabled)
            onCreateNode(vx, vy, vz, node);

        if (node != NULL) {
            m_scene->addChild(node);
            redraw();
            draw();
        }
    }

    // Handle selection

    if ((m_editMode == WidgetMode::SelectPosition) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT)) {
        double vx, vy, vz;
        Node *node = NULL;
        Vec3d pos;

        this->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        onSelectPosition(vx, vy, vz);
    }

    if (((m_editMode == WidgetMode::SelectVolume) || (m_editMode == WidgetMode::BoxSelection)) &&
        (mouseButton() == GLFW_MOUSE_BUTTON_LEFT)) {
        double vx, vy, vz;
        Node *node = NULL;
        Vec3d pos;

        this->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        if (m_clickNumber == 0) {
            m_volumeStart[0] = vx;
            m_volumeStart[1] = vy;
            m_volumeStart[2] = vz;

            m_volumeEnd[0] = vx;
            m_volumeEnd[1] = vy;
            m_volumeEnd[2] = vz;

            ivf::Point3d p0, p1;
            p0.setComponents(m_volumeStart);
            p1.setComponents(m_volumeEnd);
            m_volumeSelection->setSize(&p0, &p1);

            m_volumeSelection->setState(Shape::OS_ON);
            m_clickNumber++;
        }
        else if (m_clickNumber == 1) {
            m_volumeEnd[0] = vx;
            m_volumeEnd[1] = vy;
            m_volumeEnd[2] = vz;

            if (m_volumeStart[1] == m_volumeEnd[1]) {
                m_volumeStart[1] = -1e300;
                m_volumeEnd[1] = 1e300;
            }

            if (m_editMode == WidgetMode::SelectVolume)
                onSelectVolume(m_volumeStart[0], m_volumeStart[1], m_volumeStart[2], m_volumeEnd[0], m_volumeEnd[1],
                               m_volumeEnd[2]);
            else
                this->selectAllBox();

            m_volumeSelection->setState(Shape::OS_OFF);
            m_clickNumber = 0;
        }
    }

    if (m_editMode == WidgetMode::CreateLine) {
        if (m_selectedShapes->getSize() < 2) {
            if (m_selectedShape != NULL) {
                if (m_selectedShape->isClass("Node")) {
                    m_selectedShape->setSelect(GLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                }
                redraw();
                draw();
            }
            else {
                m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
                m_selectedShapes->clear();
                redraw();
                draw();
            }
        }

        if (m_selectedShapes->getSize() == 2) {
            this->createLine();
            m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
            m_selectedShapes->clear();
            redraw();
            draw();
        }
    }

    // Call onMouse event handler

    onMouse(x, y);
}

void IvfViewWindow::doKeyboard(int key)
{
    if (isShiftDown())
        m_scene->lockCursor();
    else
        m_scene->unlockCursor();

    onKeyboard(key);
}

void IvfViewWindow::doKeyboardReleased(int key)
{
    if (isShiftDown())
        m_scene->lockCursor();
    else
        m_scene->unlockCursor();
}

void IvfViewWindow::doShortcut(ModifierKey modifier, int key)
{
    onShortcut(modifier, key);
}

void IvfViewWindow::onMove(ivf::Composite *selectedShapes, double &dx, double &dy, double &dz, bool &doit)
{
}

void IvfViewWindow::onMoveCompleted()
{
}

void IvfViewWindow::onSelectFilter(ivf::Shape *shape, bool &select)
{
}

void IvfViewWindow::onHighlightFilter(ivf::Shape *, bool &highlight)
{
}

void IvfViewWindow::onMotion(int x, int y)
{
}

void IvfViewWindow::onPassiveMotion(int x, int y)
{
}

void IvfViewWindow::onMouse(int x, int y)
{
}

void IvfViewWindow::onMouseDown(int x, int y)
{
}

void IvfViewWindow::onMouseUp(int x, int y)
{
}

void IvfViewWindow::onKeyboard(int key)
{
}

void IvfViewWindow::onSelectVolume(double x0, double y0, double z0, double x1, double y1, double yz)
{
}

ivf::Shape *IvfViewWindow::onPick(int x, int y)
{
    return nullptr;
}

void IvfViewWindow::onPostRender()
{
}

void IvfViewWindow::onPreRender()
{
}

void IvfViewWindow::onInitImGui()
{
}

void IvfViewWindow::onDrawImGui()
{
}
