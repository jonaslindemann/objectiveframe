 #include "IvfViewWindow.h"

#include <iostream>

#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#undef USE_OFFSCREEN_RENDERING

using namespace std;
using namespace ivf;

std::shared_ptr<IvfViewWindow> IvfViewWindow::create(int width, int height, const std::string title, GLFWmonitor* monitor, GLFWwindow* shared)
{
    return std::make_shared<IvfViewWindow>(width, height, title, monitor, shared);
}

IvfViewWindow::IvfViewWindow(int width, int height, const std::string title, GLFWmonitor* monitor, GLFWwindow* shared)
    :GLFWWindow(width, height, title, monitor, shared)
{
    // State variables

    m_currentButton = ButtonState::bsNoButton;
    m_currentModifier = ButtonState::bsNoButton;

    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    m_snapToGrid = true;
    m_selectedShape = NULL;
    m_editMode = WidgetMode::ViewPan;
    m_clickNumber = 0;
    m_nNodes = 0;
    m_nLines = 0;
    m_doOverlay = false;
    m_editEnabled = true;
    m_selectEnabled = true;
    m_lastShape = NULL;
    m_initDone = false;
    m_snapToGrid = true;

    m_mouseUpdate = false;

    m_workspaceSize = 10.0f;

    // Create default camera

    m_camera = Camera::create();
    m_camera->setPosition(0.0, m_workspaceSize / 8.0, -m_workspaceSize / 2.0);
    m_camera->setPerspective(45.0, 0.1, 100.0);

    // Create scene

    m_scene = Workspace::create();
    m_scene->setView(m_camera);
    m_scene->disableCursor();
    //m_scene->getCurrentPlane()->getCursor()->setCursorType(CIvfCursor::CT_LINE_CURSOR);

    // Create selected shapes list.

    m_selectedShapes = Composite::create();
    m_selectedShapes->setUseReference(false);

    m_quit = false;
}

IvfViewWindow::~IvfViewWindow()
{
    onDestroy();
}

void IvfViewWindow::deleteAll()
{
    m_scene->deleteAll();
    this->redraw();
    m_selectedShapes->clear();
}

// ------------------------------------------------------------
void IvfViewWindow::deleteSelected()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    for (i = 0; i < m_selectedShapes->getSize(); i++)
    {
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

// ------------------------------------------------------------
void IvfViewWindow::deleteSelectedKeep()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    vector<ShapePtr> remainingShapes;

    for (i = 0; i < m_selectedShapes->getSize(); i++)
    {
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

// ------------------------------------------------------------
void IvfViewWindow::createLine()
{
    if (m_selectedShapes->getSize() == 2)
    {
        Node* node1 = static_cast<Node*>(m_selectedShapes->getChild(0));
        Node* node2 = static_cast<Node*>(m_selectedShapes->getChild(1));

        if ((node1->isClass("Node")) && (node2->isClass("Node")))
        {
            Shape* solidLine = nullptr;
            onCreateLine(node1, node2, solidLine);
            if (solidLine != nullptr)
            {
                addToScene(solidLine);
            }
        }
    }
}

// ------------------------------------------------------------
void IvfViewWindow::addToScene(Shape* shape)
{
    m_scene->addChild(shape);
}

// ------------------------------------------------------------
void IvfViewWindow::centerSelected()
{
    if (m_selectedShapes->getSize() == 1)
    {
        Shape* shape = static_cast<Shape*>(m_selectedShapes->getChild(0));
        double x, y, z;
        shape->getPosition(x, y, z);
        m_camera->setTarget(x, y, z);
        redraw();
    }
}

// ------------------------------------------------------------
void IvfViewWindow::resetView()
{
    m_camera->setPosition(0.0, m_workspaceSize / 8.0, -m_workspaceSize);
    m_camera->setTarget(0.0, 0.0, 0.0);
    redraw();
}

// ------------------------------------------------------------
void IvfViewWindow::clearSelection()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = nullptr;

    // Should onDeSelect be called ???

    // onDeSelect();
    redraw();
}

// ------------------------------------------------------------
void IvfViewWindow::selectAll()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    int i;

    Shape* shape;
    auto scene = this->getScene()->getComposite();
    bool select;

    for (i = 0; i < scene->getSize(); i++)
    {
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

// ------------------------------------------------------------
// Get/set methods
// ------------------------------------------------------------

void IvfViewWindow::setEditMode(WidgetMode mode)
{
    m_editMode = mode;

    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    if (getEditMode() == WidgetMode::Select)
    {
        m_selectedShape = NULL;
        m_scene->disableCursor();
    };

    if (getEditMode() == WidgetMode::CreateLine)
    {
        clearSelection();
        m_scene->disableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
    }

    if (getEditMode() == WidgetMode::CreateNode)
    {
        clearSelection();
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
        m_scene->unlockCursor();
    }

    if (getEditMode() == WidgetMode::Move)
    {
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_scene->unlockCursor();
    }

    if (getEditMode() == WidgetMode::ViewPan)
    {
        m_scene->disableCursor();
    }

    if (getEditMode() == WidgetMode::ViewZoom)
    {
        m_scene->disableCursor();
    }

    redraw();
}

// ------------------------------------------------------------
WidgetMode IvfViewWindow::getEditMode()
{
    return m_editMode;
}

// ------------------------------------------------------------
Workspace* IvfViewWindow::getScene()
{
    return m_scene;
}

void IvfViewWindow::quit()
{
    m_quit = true;
}

// ------------------------------------------------------------
Camera* IvfViewWindow::getCamera()
{
    return m_camera;
}

// ------------------------------------------------------------
void IvfViewWindow::setWorkspace(double size, bool resetCamera)
{
    m_workspaceSize = size;

    m_scene->setSize(size);

    if (resetCamera)
        m_camera->setPosition(0.0, m_workspaceSize / 8.0, -m_workspaceSize / 2.0);

    m_controlSize = m_workspaceSize / 50.0;
}

// ------------------------------------------------------------
double IvfViewWindow::getWorkspace()
{
    return m_scene->getSize();
}

// ------------------------------------------------------------
Composite* IvfViewWindow::getSelectedShapes()
{
    return m_selectedShapes;
}

bool IvfViewWindow::isInitialized()
{
    return false;
}

void IvfViewWindow::disableRedrawTimer()
{
}

void IvfViewWindow::enableRedrawTimer()
{
}

bool IvfViewWindow::isRedrawTimerEnabled()
{
    return false;
}

// ------------------------------------------------------------
void IvfViewWindow::setUseOverlay(bool flag)
{
    m_doOverlay = flag;
    this->redraw();
}

// ------------------------------------------------------------
bool IvfViewWindow::getUseOverlay()
{
    return m_doOverlay;
}

// ------------------------------------------------------------
void IvfViewWindow::setEditEnabled(bool flag)
{
    m_editEnabled = flag;
}

// ------------------------------------------------------------
void IvfViewWindow::setSelectEnable(bool flag)
{
    m_selectEnabled = flag;
}

// ------------------------------------------------------------
void IvfViewWindow::setSnapToGrid(bool flag)
{
    m_snapToGrid = flag;
    //m_scene->setSnapToGrid(m_snapToGrid);
}

// ------------------------------------------------------------
bool IvfViewWindow::getSnapToGrid()
{
    return m_snapToGrid;
}

// ------------------------------------------------------------
int IvfViewWindow::getManipulatorMode()
{
    return m_manipulatorMode;
}

// ------------------------------------------------------------
ButtonState IvfViewWindow::getCurrentMouseButton()
{
    return m_currentButton;
}

// ------------------------------------------------------------
ButtonState IvfViewWindow::getCurrentModifier()
{
    m_currentModifier = ButtonState::bsNoButton;

    /*
    if (Fl::get_key(FL_Shift_L))
        m_currentModifier = ButtonState::Shift;
    if (Fl::get_key(FL_Shift_R))
        m_currentModifier = ButtonState::Shift;
    if (Fl::get_key(FL_Control_L))
        m_currentModifier = ButtonState::Ctrl;
    if (Fl::get_key(FL_Control_R))
        m_currentModifier = ButtonState::Ctrl;
    if (Fl::get_key(FL_Alt_R))
        m_currentModifier = ButtonState::Alt;
    if (Fl::get_key(FL_Alt_L))
        m_currentModifier = ButtonState::Alt;
    */

    return m_currentModifier;
}

void IvfViewWindow::onGlfwKey(int key, int scancode, int action, int mods)
{
}

void IvfViewWindow::onGlfwMousePosition(double x, double y)
{
    doMouse(mouseX(), mouseY());
}

void IvfViewWindow::onGlfwMouseButton(int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        doMouseDown(mouseX(), mouseY());
    }
    else
    {
        doMouseUp(mouseX(), mouseY());
    }
}

void IvfViewWindow::onGlfwResize(int width, int height)
{
    doResize(width, height);
}

void IvfViewWindow::onGlfwDraw()
{
    if (!m_initDone)
    {
        onInitContext();
        m_lighting = Lighting::getInstance();
        m_lighting->setTwoSide(true);

        auto light = m_lighting->getLight(0);
        light->setLightPosition(0.0f, 0.5f, 1.0f, 0.0f);
        light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
        light->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
        light->enable();

        // Create and bind the FBO

#ifdef USE_OFFSCREEN_RENDERING
        initOffscreenBuffers();
        updateOffscreenBuffers();
#endif

        onInit();

        m_initDone = true;
        m_prevWindowSize[0] = width();
        m_prevWindowSize[1] = height();
    }

    // Set up camera

    m_camera->setViewPort(width(), height());
    m_camera->initialize();

#ifdef USE_OFFSCREEN_RENDERING

    if ((m_prevWindowSize[0] != pixel_w()) || (m_prevWindowSize[1] != h()))
    {
        m_prevWindowSize[0] = pixel_w();
        m_prevWindowSize[1] = pixel_h();

        updateOffscreenBuffers();
    }

    bindOffscreenBuffers();

#endif
    // Drawing code

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    //m_camera->rotateAbsolute(m_angleX / 100.0, m_ang)
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

    m_scene->render();

    glPushMatrix();

    if (m_doOverlay)
    {
        glPushAttrib(GL_ENABLE_BIT);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)width(), (double)width(), 0.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        onOverlay();

        glPopAttrib();

        m_camera->setViewPort(width(), width());
        m_camera->initialize();
    }

    glPopMatrix();


#ifdef USE_OFFSCREEN_RENDERING
    blitOffscreenBuffers();
#endif

    //this->doDrawImGui();
}

void IvfViewWindow::redraw()
{
}

void IvfViewWindow::doDrawImGui()
{

}

void IvfViewWindow::onInit()
{
}

void IvfViewWindow::onDestroy()
{
}

void IvfViewWindow::onCreateNode(double x, double y, double z, ivf::Node*& newNode)
{
}

void IvfViewWindow::onCreateLine(ivf::Node* node1, ivf::Node* node2, ivf::Shape*& newLine)
{
}

void IvfViewWindow::onSelect(ivf::Composite* selectedShapes)
{
}

void IvfViewWindow::onDeSelect()
{
}

void IvfViewWindow::onCoordinate(double x, double y, double z)
{
}

void IvfViewWindow::onDeleteShape(ivf::Shape* shape, bool& doit)
{
}

void IvfViewWindow::onInitContext()
{
}

void IvfViewWindow::onOverlay()
{
}

void IvfViewWindow::onHighlightShape(ivf::Shape* shape)
{
}

void IvfViewWindow::doMouseUp(int x, int y)
{
    // Call onMouseUp event method

    //if (getEditMode() == WidgetMode::Move)
    //    onMoveCompleted();

    this->getScene()->showCursor();
    onMouseUp(x, y);
}

void IvfViewWindow::doMouseDown(int x, int y)
{
    m_moveStart = true;
    onMouseDown(x, y);
}

void IvfViewWindow::doPassiveMotion(int x, int y)
{
}

void IvfViewWindow::doMotion(int x, int y)
{
}

void IvfViewWindow::doMouse(int x, int y)
{
    cout << "doMouse(" << x << ", " << y << ")" << endl;

    // Store mouse down position.

    Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
    pos.getComponents(m_startPos[0], m_startPos[1], m_startPos[2]);

    if ((m_editMode == WidgetMode::Select) && (m_selectEnabled))
    {
        if (m_selectedShape != NULL)
        {
            if (m_selectedShape->getSelect() != GLBase::SS_ON)
            {
                bool select = true;
                onSelectFilter(m_selectedShape, select);
                if (select)
                {
                    m_selectedShape->setSelect(GLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                    onSelect(m_selectedShapes);
                }
                redraw();
            }
        }
        else
        {
            m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
            m_selectedShapes->clear();
            onDeSelect();
            redraw();
        }
    }

    // Handle node creation

    if ((m_editMode == WidgetMode::CreateNode) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT))
    {
        double vx, vy, vz;
        Node* node = NULL;
        Vec3d pos;

        m_scene->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        if (m_editEnabled)
            onCreateNode(vx, vy, vz, node);

        if (node != NULL)
        {
            m_scene->addChild(node);
            redraw();
        }
    }

    // Handle selection

    if ((m_editMode == WidgetMode::SelectPosition) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT))
    {
        double vx, vy, vz;
        Node* node = NULL;
        Vec3d pos;

        m_scene->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        //onSelectPosition(vx, vy, vz);
    }

    if (((m_editMode == WidgetMode::SelectVolume) || (m_editMode == WidgetMode::BoxSelection)) && (mouseButton() == GLFW_MOUSE_BUTTON_LEFT) > 0)
    {
        double vx, vy, vz;
        Node* node = NULL;
        Vec3d pos;

        m_scene->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(vx, vy, vz);

        if (m_clickNumber == 0)
        {
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
        else if (m_clickNumber == 1)
        {
            m_volumeEnd[0] = vx;
            m_volumeEnd[1] = vy;
            m_volumeEnd[2] = vz;

            if (m_volumeStart[1] == m_volumeEnd[1])
            {
                m_volumeStart[1] = -1e300;
                m_volumeEnd[1] = 1e300;
            }

            //if (m_editMode == WidgetMode::SelectVolume)
            //    onSelectVolume(m_volumeStart[0], m_volumeStart[1], m_volumeStart[2], m_volumeEnd[0], m_volumeEnd[1], m_volumeEnd[2]);
            //else
            //    this->selectAllBox();

            m_volumeSelection->setState(Shape::OS_OFF);
            m_clickNumber = 0;
        }

        // onSelectPosition(vx, vy, vz);
    }

    if (m_editMode == WidgetMode::CreateLine)
    {
        if (m_selectedShapes->getSize() < 2)
        {
            if (m_selectedShape != NULL)
            {
                if (m_selectedShape->isClass("Node"))
                {
                    m_selectedShape->setSelect(GLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                }
                redraw();
            }
            else
            {
                m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
                m_selectedShapes->clear();
                redraw();
            }
        }

        if (m_selectedShapes->getSize() == 2)
        {
            this->createLine();
            m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
            m_selectedShapes->clear();
            redraw();
        }
    }

    // Call onMouse event handler

    onMouse(x, y);
}

void IvfViewWindow::doKeyboard(int key)
{
    // Call onMouseUp event method

    onKeyboard(key);
}

void IvfViewWindow::onMove(ivf::Composite* selectedShapes, double& dx, double& dy, double& dz, bool& doit)
{
}

void IvfViewWindow::onSelectFilter(ivf::Shape* shape, bool& select)
{
}

void IvfViewWindow::onHighlightFilter(ivf::Shape*, bool& highlight)
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

void IvfViewWindow::onPostRender()
{
}

void IvfViewWindow::onPreRender()
{
}

void IvfViewWindow::onDrawImGui()
{
}
