//
// Copyright 1999-2018 by Structural Mechanics, Lund University.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "ivf@byggmek.lth.se".
//   

#include "FltkWidget.h"

#include <FL/x.H>
#include <FL/gl.h>
#include <FL/fl_draw.H>

#include <glad/glad.h>

#ifdef WIN32
#include <Windows.h>
#endif

#include <GL/glu.h>


#define USE_OFFSCREEN_RENDERING

using namespace ivf;

void redrawCallback(void* view)
{
    FltkWidget* imguiView;

    imguiView = (FltkWidget*)view;
    imguiView->redraw();
    Fl::repeat_timeout(1.0 / 60.0, redrawCallback, (void*)view);
}

// ------------------------------------------------------------
// Constructor/desctructor
// ------------------------------------------------------------

FltkWidget::FltkWidget(int X, int Y, int W, int H, const char *L) :
    Fl_Gl_Window(X, Y, W, H, L), ImGuiFLTKImpl()
{
    // State variables

    int oldMode = this->mode();
    this->mode(FL_RGB8 | FL_DOUBLE | FL_STENCIL | FL_MULTISAMPLE);
    int newMode = this->mode();

    m_currentButton = IVF_NO_BUTTON;
    m_currentModifier = IVF_NO_BUTTON;
    m_currentState = 0;

    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    m_snapToGrid = true;
    m_selectedShape = NULL;
    m_editMode = IVF_VIEW_PAN;
    m_clickNumber = 0;
    m_nNodes = 0;
    m_nLines = 0;
    m_doOverlay = false;
    m_editEnabled = true;
    m_selectEnabled = true;
    m_lastShape = NULL;
    m_initDone = false;
    m_snapToGrid = true;

    m_workspaceSize = 10.0f;

    m_showDemoWindow = true;

    // Create default camera

    m_camera = Camera::create();
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize/2.0);
    m_camera->setPerspective(45.0, 0.1, 100.0);

    // Create scene

    m_scene = Workspace::create();
    m_scene->setView(m_camera);
    m_scene->disableCursor();
	//m_scene->getCurrentPlane()->getCursor()->setCursorType(CIvfCursor::CT_LINE_CURSOR);

    // Create selected shapes list.

    m_selectedShapes = Composite::create();
    m_selectedShapes->setUseReference(false);

    Fl::repeat_timeout(1.0 / 60.0, redrawCallback, (void*)this);
}

// ------------------------------------------------------------
FltkWidget::~FltkWidget()
{
    onDestroy();
}

// ------------------------------------------------------------
// Methods
// ------------------------------------------------------------

void FltkWidget::deleteAll()
{
    m_scene->deleteAll();
    this->redraw();
    m_selectedShapes->clear();
}

// ------------------------------------------------------------
void FltkWidget::deleteSelected()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    for (i = 0; i<m_selectedShapes->getSize(); i++ )
    {
        auto shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape,doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            shape->setSelect(GLBase::SS_OFF);
    }

    m_selectedShapes->clear();
    redraw();
}

// ------------------------------------------------------------
void FltkWidget::deleteSelectedKeep()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    vector<ShapePtr> remainingShapes;

    for (i = 0; i<m_selectedShapes->getSize(); i++ )
    {
        auto shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape,doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            remainingShapes.push_back(ShapePtr(shape));
    }

    m_selectedShapes->clear();

    for (i=0; i<remainingShapes.size(); i++)
        m_selectedShapes->addChild(remainingShapes[i]);

    redraw();
}

// ------------------------------------------------------------
void FltkWidget::createLine()
{
    if (m_selectedShapes->getSize()==2)
    {
        Node* node1 = static_cast<Node*>(m_selectedShapes->getChild(0));
        Node* node2 = static_cast<Node*>(m_selectedShapes->getChild(1));

        if ( (node1->isClass("Node") )&&( node2->isClass("Node") ))
        {
            Shape* solidLine = nullptr;
            onCreateLine(node1,node2,solidLine);
            if (solidLine!=nullptr)
            {
                addToScene(solidLine);
            }
        }
    }
}

// ------------------------------------------------------------
void FltkWidget::addToScene(Shape* shape)
{
    m_scene->addChild(shape);
}

// ------------------------------------------------------------
void FltkWidget::centerSelected()
{
    if (m_selectedShapes->getSize()==1)
    {
        Shape* shape = static_cast<Shape*>(m_selectedShapes->getChild(0));
        double x, y, z;
        shape->getPosition(x, y, z);
        m_camera->setTarget(x, y, z);
        redraw();
    }
}

// ------------------------------------------------------------
void FltkWidget::resetView()
{
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize);
    m_camera->setTarget(0.0, 0.0, 0.0);
    redraw();
}

// ------------------------------------------------------------
void FltkWidget::clearSelection()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = nullptr;

    // Should onDeSelect be called ???

    // onDeSelect();
    redraw();
}

// ------------------------------------------------------------
void FltkWidget::selectAll()
{
    m_selectedShapes->setSelectChildren(GLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    int i;

    Shape* shape;
    auto scene = this->getScene()->getComposite();
    bool select;

    for (i=0; i<scene->getSize(); i++)
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

void FltkWidget::setEditMode(int mode)
{
    m_editMode = mode;

    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    if ( getEditMode() == IVF_SELECT )
    {
        m_selectedShape = NULL;
        m_scene->disableCursor();
    };

    if ( getEditMode() == IVF_CREATE_LINE)
    {
        clearSelection();
        m_scene->disableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
    }

    if ( getEditMode() == IVF_CREATE_NODE)
    {
        clearSelection();
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_selectedShapes->clear();
        m_scene->unlockCursor();
    }

    if ( getEditMode() == IVF_MOVE)
    {
        m_scene->enableCursor();
        m_clickNumber = 0;
        m_scene->unlockCursor();
    }

    if ( getEditMode() == IVF_VIEW_PAN)
    {
        m_scene->disableCursor();
    }

    if ( getEditMode() == IVF_VIEW_ZOOM)
    {
        m_scene->disableCursor();
    }

    redraw();
}

// ------------------------------------------------------------
int FltkWidget::getEditMode()
{
    return m_editMode;
}

// ------------------------------------------------------------
Workspace* FltkWidget::getScene()
{
    return m_scene;
}

// ------------------------------------------------------------
Camera* FltkWidget::getCamera()
{
    return m_camera;
}

// ------------------------------------------------------------
void FltkWidget::setWorkspace(double size)
{
    m_workspaceSize = size;

    m_scene->setSize(size);
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize/2.0);
    m_controlSize = m_workspaceSize/50.0;
}

// ------------------------------------------------------------
double FltkWidget::getWorkspace()
{
    return m_scene->getSize();
}

// ------------------------------------------------------------
Composite* FltkWidget::getSelectedShapes()
{
    return m_selectedShapes;
}

// ------------------------------------------------------------
void FltkWidget::setUseOverlay(bool flag)
{
    m_doOverlay = flag;
    this->redraw();
}

// ------------------------------------------------------------
bool FltkWidget::getUseOverlay()
{
    return m_doOverlay;
}

// ------------------------------------------------------------
void FltkWidget::setEditEnabled(bool flag)
{
    m_editEnabled = flag;
}

// ------------------------------------------------------------
void FltkWidget::setSelectEnable(bool flag)
{
    m_selectEnabled = flag;
}

// ------------------------------------------------------------
void FltkWidget::setSnapToGrid(bool flag)
{
    m_snapToGrid = flag;
    //m_scene->setSnapToGrid(m_snapToGrid);
}

// ------------------------------------------------------------
bool FltkWidget::getSnapToGrid()
{
    return m_snapToGrid;
}

// ------------------------------------------------------------
int FltkWidget::getManipulatorMode()
{
    return m_manipulatorMode;
}

// ------------------------------------------------------------
int FltkWidget::getCurrentMouseButton()
{
    return m_currentButton;
}

// ------------------------------------------------------------
int FltkWidget::getCurrentModifier()
{
    m_currentModifier = IVF_NO_BUTTON;

    if (Fl::get_key(FL_Shift_L))
        m_currentModifier = IVF_SHIFT;
    if (Fl::get_key(FL_Shift_R))
        m_currentModifier = IVF_SHIFT;
    if (Fl::get_key(FL_Control_L))
        m_currentModifier = IVF_CTRL;
    if (Fl::get_key(FL_Control_R))
        m_currentModifier = IVF_CTRL;
    if (Fl::get_key(FL_Alt_R))
        m_currentModifier = IVF_ALT;
    if (Fl::get_key(FL_Alt_L))
        m_currentModifier = IVF_ALT;

    return m_currentModifier;
}

// ------------------------------------------------------------
// Implemented FLTK methods
// ------------------------------------------------------------


void FltkWidget::initOffscreenBuffers()
{
	glGenFramebuffers(1, &m_multiFbo);
	glGenTextures(1, &m_screenTexture);
	glGenRenderbuffers(1, &m_colorBuffer);
	glGenRenderbuffers(1, &m_depthBuffer);
}

void FltkWidget::updateOffscreenBuffers()
{
	// Create multisample texture
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_screenTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, w(), h(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_multiFbo);

	glBindRenderbuffer(GL_RENDERBUFFER, m_colorBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, w(), h());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w(), h());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

	// Bind Texture assuming we have created a texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenTexture, 0);

}

void FltkWidget::bindOffscreenBuffers()
{
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_screenTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_multiFbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);

	// Enable multisampling
	glEnable(GL_MULTISAMPLE);
}

void FltkWidget::blitOffscreenBuffers()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_multiFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, w(), h(), 0, 0, w(), h(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FltkWidget::draw()
{
    if (this->changed())
    {
        m_scene->getComposite()->refresh();
        this->clear_changed();
    }

    if (!valid())
    {
        // Call initialisation callback

        onInitContext();

        if (!m_initDone)
        {
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
			m_prevWindowSize[0] = w();
			m_prevWindowSize[1] = h();
		}

        // Set up camera

        m_camera->setViewPort(w(), h());
        m_camera->initialize();

    }

#ifdef USE_OFFSCREEN_RENDERING

	if ((m_prevWindowSize[0] != w()) || (m_prevWindowSize[1] != h()))
	{
		m_prevWindowSize[0] = w();
		m_prevWindowSize[1] = h();

		updateOffscreenBuffers();
	}

	bindOffscreenBuffers();

#endif
    // Drawing code

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    m_camera->rotatePositionY(m_angleX/100.0);
    m_camera->rotatePositionX(m_angleY/100.0);

    m_camera->moveSideways(m_moveX*m_workspaceSize/1000.0);
    m_camera->moveVertical(m_moveY*m_workspaceSize/1000.0);
    m_camera->moveDepth(m_zoomY*m_workspaceSize/500.0);

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
        glOrtho(0.0, (double)w(), (double)h(), 0.0, 0.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        onOverlay();

        glPopAttrib();

        m_camera->setViewPort(w(), h());
        m_camera->initialize();
    }

    glPopMatrix();


#ifdef USE_OFFSCREEN_RENDERING
    blitOffscreenBuffers();
#endif

    this->doDrawImGui();
}

// ------------------------------------------------------------
int FltkWidget::handle(int event)
{
#ifndef __APPLE__
	static int first = 1;
	if (first && event == FL_SHOW && shown()) {
		first = 0;
		make_current();

        gladLoadGL();

        this->doInitImGui(w(), h());

        this->focus(this);
	}
#endif

	switch(event)
    {
    case FL_ENTER:
        Fl::belowmouse(this);
        //cout << "FL_ENTER" << endl;
        return 1;
    case FL_LEAVE:
        m_angleX = 0.0f;
        m_angleY = 0.0f;
        m_moveX = 0.0f;
        m_moveY = 0.0f;
        m_zoomX = 0.0f;
        m_zoomY = 0.0f;
        return 1;
    case FL_MOVE:
        m_beginX = Fl::event_x();
        m_beginY = Fl::event_y();
        if (!Fl::get_key(FL_Shift_L))
            m_scene->unlockCursor();
        this->doPassiveMotion(m_beginX, m_beginY);
        this->doImGuiMove();
        return 1;
    case FL_PUSH:
        m_beginX = Fl::event_x();
        m_beginY = Fl::event_y();

        m_currentModifier = IVF_NO_BUTTON;

        if (Fl::get_key(FL_Shift_L))
            m_currentModifier = IVF_SHIFT;
        if (Fl::get_key(FL_Shift_R))
            m_currentModifier = IVF_SHIFT;
        if (Fl::get_key(FL_Control_L))
            m_currentModifier = IVF_CTRL;
        if (Fl::get_key(FL_Control_R))
            m_currentModifier = IVF_CTRL;
        if (Fl::get_key(FL_Alt_L))
            m_currentModifier = IVF_ALT;
        if (Fl::get_key(FL_Alt_R))
            m_currentModifier = IVF_ALT;

        if (!Fl::get_key(FL_Shift_L))
            m_scene->unlockCursor();
        if (Fl::event_button()==FL_LEFT_MOUSE)
            m_currentButton = IVF_BUTTON1;
        if (Fl::event_button()==FL_MIDDLE_MOUSE)
            m_currentButton = IVF_BUTTON2;
        if (Fl::event_button()==FL_RIGHT_MOUSE)
            m_currentButton = IVF_BUTTON3;

        if (Fl::get_key(FL_Shift_L))
            cout << "SHIFT_LEFT" << endl;
        if (Fl::get_key(FL_Shift_R))
            cout << "SHIFT_RIGHT" << endl;
        if (Fl::get_key(FL_Control_L))
            cout << "CONTROL_LEFT" << endl;
        if (Fl::get_key(FL_Control_R))
            cout << "CONTROL_RIGHT" << endl;
        if (Fl::get_key(FL_Alt_L))
            cout << "ALT_LEFT" << endl;
        if (Fl::get_key(FL_Alt_R))
            cout << "ALT_RIGHT" << endl;

        if (Fl::event_button()==FL_LEFT_MOUSE)
            cout << "LEFT_MOUSE" << endl;
        if (Fl::event_button()==FL_MIDDLE_MOUSE)
            cout << "MIDDLE_MOUSE" << endl;
        if (Fl::event_button()==FL_RIGHT_MOUSE)
            cout << "RIGHT_MOUSE" << endl;


        this->doMouseDown(m_beginX, m_beginY);
        this->doMouse(m_beginX, m_beginY);
        this->doImGuiPush();
        return 1;
    case FL_DRAG:
        if (m_editMode!=IVF_MANIPULATE)
            if (Fl::get_key(FL_Shift_L)==FALSE)
                m_scene->unlockCursor();
        if (Fl::event_state()==FL_BUTTON1)
            m_currentButton = IVF_BUTTON1;
        if (Fl::event_state()==FL_BUTTON2)
            m_currentButton = IVF_BUTTON2;
        if (Fl::event_state()==FL_BUTTON3)
            m_currentButton = IVF_BUTTON3;
        this->doMotion(Fl::event_x(),Fl::event_y());
        //cout << "FL_DRAG" << endl;
        this->doImGuiDrag();
        return 1;
    case FL_RELEASE:
        this->doMouseUp(Fl::event_x(), Fl::event_y());
        m_currentButton = IVF_NO_BUTTON;
        this->doImGuiRelease();
        return 1;
    case FL_FOCUS:

        return 1;
    case FL_UNFOCUS:

        return 1;
    case FL_KEYBOARD:
        if (Fl::event_key()==FL_Shift_L)
        {
            if (!m_scene->isCursorLocked())
                m_scene->lockCursor();
            this->doImGuiKeyboard();
            return 1;
        }
        else
        {
            doKeyboard(Fl::event_key());
            this->doImGuiKeyboard();
            return 0;
        }
    case FL_KEYUP:
        this->doImGuiKeyUp();
        return 1;

    case FL_SHORTCUT:

        return 0;
    default:

        return 0;
    }
}

void FltkWidget::resize(int x, int y, int w, int h)
{
    Fl_Gl_Window::resize(x, y, w, h); 
    if (this->isImGuiInitialised())
        this->doImGuiResize(w, h);
}

// ------------------------------------------------------------
// ImGui events
// ------------------------------------------------------------

void FltkWidget::onDrawImGui()
{
    ImGui::ShowDemoWindow(&m_showDemoWindow);
    ImGui::Render();

    if (ImGui::IsMouseDown(0))
        cout << "Mouse0 down" << endl;
    if (ImGui::IsMouseDown(1))
        cout << "Mouse1 down" << endl;
    if (ImGui::IsMouseDown(2))
        cout << "Mouse2 down" << endl;
}

void FltkWidget::onInitImGui()
{
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 20);
    //ImFont* font2 = io.Fonts->AddFontFromFileTTF("anotherfont.otf", 13);
}

// ------------------------------------------------------------
// Event methods
// ------------------------------------------------------------

void FltkWidget::doMouse(int x, int y)
{
    // Store mouse down position.

    Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
    pos.getComponents(m_startPos[0], m_startPos[1], m_startPos[2]);

    if ((m_editMode == IVF_SELECT)&&(m_selectEnabled))
    {
        if (m_selectedShape!=NULL)
        {
            if (m_selectedShape->getSelect()!=GLBase::SS_ON)
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

    if ((m_editMode == IVF_CREATE_NODE)&&(Fl::event_state(FL_BUTTON1)>0))
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

    if (m_editMode == IVF_CREATE_LINE)
    {
        if (m_selectedShapes->getSize()<2)
        {
            if (m_selectedShape!=NULL)
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

        if (m_selectedShapes->getSize()==2)
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

void FltkWidget::onMouse(int x, int y)
{

}

// ------------------------------------------------------------
void FltkWidget::doMotion(int x, int y)
{
    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    this->getScene()->showCursor();
    //if ( (getEditMode()>=IVF_VIEW) && (getEditMode()<IVF_CREATE) )
    {
        if (Fl::event_state(FL_BUTTON3)>0) {
            //if ((getEditMode()==IVF_VIEW_ZOOM)||(getEditMode()==IVF_VIEW_PAN))
        {
                this->getScene()->hideCursor();
                if (getCurrentModifier()==IVF_ALT)
                {
                    m_zoomX = (x - m_beginX);
                    m_zoomY = (y - m_beginY);
                }
                else if (getCurrentModifier()==IVF_SHIFT)
                {
                    m_moveX = (x - m_beginX);
                    m_moveY = (y - m_beginY);
                }
                else
                {
                    m_angleX = (x - m_beginX);
                    m_angleY = (y - m_beginY);
                }
            }
            m_beginX = x;
            m_beginY = y;

            m_scene->updateSizes();
            redraw();
        }


#ifdef OLD_VIEW_HANDLING
        if (Fl::event_state(FL_BUTTON1)>0) {
            m_angleX = (x - m_beginX);
            m_angleY = (y - m_beginY);
            m_beginX = x;
            m_beginY = y;

            m_scene->updateSizes();

            redraw();
        }
        if (Fl::event_state(FL_BUTTON3)>0) {
            if ((getEditMode()==IVF_VIEW_ZOOM)||(getEditMode()==IVF_VIEW_PAN))
            {
                if (getCurrentModifier()==IVF_NO_BUTTON)
                {
                    m_zoomX = (x - m_beginX);
                    m_zoomY = (y - m_beginY);
                }
                else if (getCurrentModifier()==IVF_SHIFT)
                {
                    m_moveX = (x - m_beginX);
                    m_moveY = (y - m_beginY);
                }
            }
            m_beginX = x;
            m_beginY = y;

            m_scene->updateSizes();
            redraw();
        }
#endif
    }

    if (getEditMode()==IVF_MOVE&&(Fl::event_state(FL_BUTTON1)>0))
    {
        m_scene->updateCursor(x, y);
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

        onMove(m_selectedShapes, dx, dy, dz, doit);

        if (doit)
        {
            for (int i=0; i<m_selectedShapes->getSize(); i++)
            {
                auto shape = m_selectedShapes->getChild(i);
                shape->getPosition(x, y, z);
                shape->setPosition(x + dx, y + dy, z + dz);
            }
            m_scene->getComposite()->refresh();
            redraw();
        }
    }

    // Call onMotion event method

    onMotion(x, y); 
}

// ------------------------------------------------------------
void FltkWidget::onMotion(int x, int y)
{

}

// ------------------------------------------------------------
void FltkWidget::doPassiveMotion(int x, int y)
{
    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    m_scene->updateCursor(x, y);

    if ((getEditMode() == IVF_SELECT)&&(m_selectEnabled))
    {
        bool needInvalidate = false;

        if (m_selectedShape!=nullptr)
        {
            m_selectedShape->setHighlight(Shape::HS_OFF);
            needInvalidate = true;
        }

        // Do pick

        m_scene->pick(x, y);

        m_selectedShape = m_scene->getSelectedShape();

        if (m_selectedShape!=NULL)
        {
            bool highlight = true;
            onHighlightFilter(m_selectedShape, highlight);
            if (highlight)
            {
                m_selectedShape->setHighlight(Shape::HS_ON);
                onHighlightShape(m_selectedShape);
            }
            needInvalidate = true;
        }

        if (needInvalidate)
            redraw();
    }


    if (getEditMode()==IVF_CREATE_NODE)
    {
        double wx, wy, wz;
        Vec3d pos;
        m_scene->updateCursor(x, y);
        pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(wx, wy, wz);
        onCoordinate(wx, wy, wz);
        redraw();
    }

    if (getEditMode()==IVF_MOVE)
    {
        double wx, wy, wz;
        m_scene->updateCursor(x, y);
        Vec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(wx, wy, wz);
        onCoordinate(wx, wy, wz);
        redraw();
    }

    if ((getEditMode() == IVF_CREATE_LINE)&&(m_selectEnabled))
    {
        bool needInvalidate = false;

        if (m_selectedShape!=nullptr)
        {
            m_selectedShape->setHighlight(Shape::HS_OFF);
            needInvalidate = true;
        }

        m_scene->pick(x, y);

        m_selectedShape = m_scene->getSelectedShape();

        if (m_selectedShape!=nullptr)
        {
            bool highlight = true;
            onHighlightFilter(m_selectedShape, highlight);
            if (highlight)
            {
                m_selectedShape->setHighlight(Shape::HS_ON);
                needInvalidate = true;
            }
        }

        if (needInvalidate)
            redraw();
    }

    // Call onPassiveMotion event method

    onPassiveMotion(x, y);
}

// ------------------------------------------------------------
void FltkWidget::onPassiveMotion(int x, int y)
{
}

// ------------------------------------------------------------
void FltkWidget::doMouseDown(int x, int y)
{
    // Call onMouseDown event method

    onMouseDown(x, y);
}

// ------------------------------------------------------------
void FltkWidget::onMouseDown(int x, int y)
{

}

// ------------------------------------------------------------
void FltkWidget::doMouseUp(int x, int y)
{
    // Call onMouseUp event method

    this->getScene()->showCursor();
    onMouseUp(x, y);
}

// ------------------------------------------------------------
void FltkWidget::doKeyboard(int key)
{
    // Call onMouseUp event method

    onKeyboard(key);
}


// ------------------------------------------------------------
void FltkWidget::onMouseUp(int x, int y)
{
}

// ------------------------------------------------------------
void FltkWidget::onCoordinate(double x, double y, double z)
{

}

// ------------------------------------------------------------
void FltkWidget::onInitContext()
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE);
    //glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    //GLint  iMultiSample = 0;
    //GLint  iNumSamples = 0;
    //glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    //glGetIntegerv(GL_SAMPLES, &iNumSamples);

    //cout << "mode after context init = " << this->mode() << endl;

    m_lighting->enable();
}

// ------------------------------------------------------------
void FltkWidget::onOverlay()
{

}

// ------------------------------------------------------------
void FltkWidget::onSelect(Composite* selectedShapes)
{

}

// ------------------------------------------------------------
void FltkWidget::onDeleteShape(Shape *shape, bool &doit)
{
    doit = true;
}

// ------------------------------------------------------------
void FltkWidget::onCreateNode(double x, double y, double z, Node* &newNode)
{

}

// ------------------------------------------------------------
void FltkWidget::onCreateLine(Node* node1, Node* node2, Shape* &newLine)
{

}

// ------------------------------------------------------------
void FltkWidget::onInit()
{

}

// ------------------------------------------------------------
void FltkWidget::onDestroy()
{

}

// ------------------------------------------------------------
void FltkWidget::onHighlightShape(Shape *shape)
{

}

// ------------------------------------------------------------
void FltkWidget::onDeSelect()
{

}

// ------------------------------------------------------------
void FltkWidget::onMove(Composite *selectedShapes, double &dx, double &dy, double &dz, bool &doit)
{
    doit = true;
}

// ------------------------------------------------------------
void FltkWidget::onSelectFilter(Shape *shape, bool &select)
{
    select = true;
}

// ------------------------------------------------------------
void FltkWidget::onHighlightFilter(Shape *, bool &highlight)
{
    highlight = true;
}

// ------------------------------------------------------------
void FltkWidget::onKeyboard(int key)
{
    
}

// ------------------------------------------------------------
void FltkWidget::onPostRender()
{

}

// ------------------------------------------------------------
void FltkWidget::onPreRender()
{

}

bool FltkWidget::isInitialized()
{
    return m_initDone;
}

