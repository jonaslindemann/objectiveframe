//
// Copyright 1999-2013 by Structural Mechanics, Lund University.
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

#include "IvfFltkWidget.h"


// ------------------------------------------------------------
// Constructor/desctructor
// ------------------------------------------------------------

CIvfFltkWidget::CIvfFltkWidget(int X, int Y, int W, int H, const char *L) :
    Fl_Gl_Window(X, Y, W, H, L)
{
    // State variables

    int oldMode = this->mode();
    this->mode(FL_RGB8 | FL_DOUBLE | FL_STENCIL | FL_MULTISAMPLE);
    int newMode = this->mode();

    std::cout << "oldMode = " << oldMode << endl;
    std::cout << "newMode = " << newMode << endl;

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

    // Create default camera

    m_camera = new CIvfCamera();
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize/2.0);
    m_camera->setPerspective(45.0, 0.1, 100.0);

    // Create scene

    m_scene = new CIvfWorkspace();
    m_scene->setView(m_camera);
    m_scene->disableCursor();

    // Create selected shapes list.

    m_selectedShapes = new CIvfComposite();
    m_selectedShapes->setUseReference(false);
}

// ------------------------------------------------------------
CIvfFltkWidget::~CIvfFltkWidget()
{
    onDestroy();
}

// ------------------------------------------------------------
// Methods
// ------------------------------------------------------------

void CIvfFltkWidget::deleteAll()
{
    m_scene->deleteAll();
    this->redraw();
    m_selectedShapes->clear();
}

// ------------------------------------------------------------
void CIvfFltkWidget::deleteSelected()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    for (i = 0; i<m_selectedShapes->getSize(); i++ )
    {
        CIvfShapePtr shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape,doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            shape->setSelect(CIvfGLBase::SS_OFF);
    }

    m_selectedShapes->clear();
    redraw();
}

// ------------------------------------------------------------
void CIvfFltkWidget::deleteSelectedKeep()
{
    int i;

    // Before anything is deleted we remove all
    // manipulators.

    vector<CIvfShapePtr> remainingShapes;

    for (i = 0; i<m_selectedShapes->getSize(); i++ )
    {
        CIvfShapePtr shape = m_selectedShapes->getChild(i);

        bool doit = false;
        if (m_editEnabled)
            onDeleteShape(shape,doit);

        if (doit == true)
            m_scene->getComposite()->removeShape(shape);
        else
            remainingShapes.push_back(CIvfShapePtr(shape));
    }

    m_selectedShapes->clear();

    for (i=0; i<remainingShapes.size(); i++)
        m_selectedShapes->addChild(remainingShapes[i]);

    redraw();
}

// ------------------------------------------------------------
void CIvfFltkWidget::createLine()
{
    if (m_selectedShapes->getSize()==2)
    {
        CIvfNode* node1 = (CIvfNode*)m_selectedShapes->getChild(0);
        CIvfNode* node2 = (CIvfNode*)m_selectedShapes->getChild(1);

        if ( (node1->isClass("CIvfNode") )&&( node2->isClass("CIvfNode") ))
        {
            CIvfShape* solidLine = NULL;
            onCreateLine(node1,node2,solidLine);
            if (solidLine!=NULL)
            {
                addToScene(solidLine);
            }
        }
    }
}

// ------------------------------------------------------------
void CIvfFltkWidget::addToScene(CIvfShape* shape)
{
    m_scene->addChild(shape);
}

// ------------------------------------------------------------
void CIvfFltkWidget::centerSelected()
{
    if (m_selectedShapes->getSize()==1)
    {
        CIvfShape* shape = (CIvfShape*)m_selectedShapes->getChild(0);
        double x, y, z;
        shape->getPosition(x, y, z);
        m_camera->setTarget(x, y, z);
        redraw();
    }
}

// ------------------------------------------------------------
void CIvfFltkWidget::resetView()
{
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize);
    m_camera->setTarget(0.0, 0.0, 0.0);
    redraw();
}

// ------------------------------------------------------------
void CIvfFltkWidget::clearSelection()
{
    m_selectedShapes->setSelectChildren(CIvfGLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    // Should onDeSelect be called ???

    // onDeSelect();
    redraw();
}

// ------------------------------------------------------------
void CIvfFltkWidget::selectAll()
{
    m_selectedShapes->setSelectChildren(CIvfGLBase::SS_OFF);
    m_selectedShapes->clear();
    m_selectedShape = NULL;

    int i;

    CIvfShape* shape;
    CIvfComposite* scene = this->getScene()->getComposite();
    bool select;

    for (i=0; i<scene->getSize(); i++)
    {
        shape = scene->getChild(i);
        select = true;
        onSelectFilter(shape, select);
        if (select)
            m_selectedShapes->addChild(shape);
    }

    m_selectedShapes->setSelectChildren(CIvfGLBase::SS_ON);
    onSelect(m_selectedShapes);

    redraw();
}

// ------------------------------------------------------------
// Get/set methods
// ------------------------------------------------------------

void CIvfFltkWidget::setEditMode(int mode)
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
int CIvfFltkWidget::getEditMode()
{
    return m_editMode;
}

// ------------------------------------------------------------
CIvfWorkspace* CIvfFltkWidget::getScene()
{
    return m_scene;
}

// ------------------------------------------------------------
CIvfCamera* CIvfFltkWidget::getCamera()
{
    return m_camera;
}

// ------------------------------------------------------------
void CIvfFltkWidget::setWorkspace(double size)
{
    m_workspaceSize = size;

    m_scene->setSize(size);
    m_camera->setPosition(0.0, m_workspaceSize/8.0,-m_workspaceSize/2.0);
    m_controlSize = m_workspaceSize/50.0;
}

// ------------------------------------------------------------
double CIvfFltkWidget::getWorkspace()
{
    return m_scene->getSize();
}

// ------------------------------------------------------------
CIvfComposite* CIvfFltkWidget::getSelectedShapes()
{
    return m_selectedShapes;
}

// ------------------------------------------------------------
void CIvfFltkWidget::setUseOverlay(bool flag)
{
    m_doOverlay = flag;
    this->redraw();
}

// ------------------------------------------------------------
bool CIvfFltkWidget::getUseOverlay()
{
    return m_doOverlay;
}

// ------------------------------------------------------------
void CIvfFltkWidget::setEditEnabled(bool flag)
{
    m_editEnabled = flag;
}

// ------------------------------------------------------------
void CIvfFltkWidget::setSelectEnable(bool flag)
{
    m_selectEnabled = flag;
}

// ------------------------------------------------------------
void CIvfFltkWidget::setSnapToGrid(bool flag)
{
    m_snapToGrid = flag;
    //m_scene->setSnapToGrid(m_snapToGrid);
}

// ------------------------------------------------------------
bool CIvfFltkWidget::getSnapToGrid()
{
    return m_snapToGrid;
}

// ------------------------------------------------------------
int CIvfFltkWidget::getManipulatorMode()
{
    return m_manipulatorMode;
}

// ------------------------------------------------------------
int CIvfFltkWidget::getCurrentMouseButton()
{
    return m_currentButton;
}

// ------------------------------------------------------------
int CIvfFltkWidget::getCurrentModifier()
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

void CIvfFltkWidget::draw()
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
            m_lighting = CIvfLighting::getInstance();
            m_lighting->setTwoSide(true);

            CIvfLight* light = m_lighting->getLight(0);
            light->setLightPosition(0.0f, 0.5f, 1.0f, 0.0f);
            light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
            light->setAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
            light->enable();

            onInit();
            m_initDone = true;
        }

        // Set up camera

        m_camera->setViewPort(w(), h());
        m_camera->initialize();

    }


    // Drawing code

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    m_camera->rotatePositionY(m_angleX/100.0);
    m_camera->rotatePositionX(m_angleY/100.0);

    m_camera->moveSideways(m_moveX*m_workspaceSize/1000.0);
    m_camera->moveVertical(m_moveY*m_workspaceSize/1000.0);
    m_camera->moveDepth(m_zoomY*m_workspaceSize/500.0);

    m_scene->render();

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
}

// ------------------------------------------------------------
int CIvfFltkWidget::handle(int event)
{
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
        return 1;
    case FL_RELEASE:
        this->doMouseUp(Fl::event_x(), Fl::event_y());
        m_currentButton = IVF_NO_BUTTON;
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
            return 1;
        }
        else
        {
            doKeyboard(Fl::event_key());
            return 0;
        }
    case FL_SHORTCUT:

        return 0;
    default:

        return 0;
    }
}

// ------------------------------------------------------------
// Event methods
// ------------------------------------------------------------

void CIvfFltkWidget::doMouse(int x, int y)
{
    // Store mouse down position.

    CIvfVec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
    pos.getComponents(m_startPos[0], m_startPos[1], m_startPos[2]);

    if ((m_editMode == IVF_SELECT)&&(m_selectEnabled))
    {
        if (m_selectedShape!=NULL)
        {
            if (m_selectedShape->getSelect()!=CIvfGLBase::SS_ON)
            {
                bool select = true;
                onSelectFilter(m_selectedShape, select);
                if (select)
                {
                    m_selectedShape->setSelect(CIvfGLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                    onSelect(m_selectedShapes);
                }
                redraw();
            }
        }
        else
        {
            m_selectedShapes->setSelectChildren(CIvfGLBase::SS_OFF);
            m_selectedShapes->clear();
            onDeSelect();
            redraw();
        }
    }

    // Handle node creation

    if ((m_editMode == IVF_CREATE_NODE)&&(Fl::event_state(FL_BUTTON1)>0))
    {
        double vx, vy, vz;
        CIvfNode* node = NULL;
        CIvfVec3d pos;

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
                if (m_selectedShape->isClass("CIvfNode"))
                {
                    m_selectedShape->setSelect(CIvfGLBase::SS_ON);
                    m_selectedShapes->addChild(m_selectedShape);
                }
                redraw();
            }
            else
            {
                m_selectedShapes->setSelectChildren(CIvfGLBase::SS_OFF);
                m_selectedShapes->clear();
                redraw();
            }
        }

        if (m_selectedShapes->getSize()==2)
        {
            this->createLine();
            m_selectedShapes->setSelectChildren(CIvfGLBase::SS_OFF);
            m_selectedShapes->clear();
            redraw();
        }
    }

    // Call onMouse event handler

    onMouse(x, y);
}

void CIvfFltkWidget::onMouse(int x, int y)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::doMotion(int x, int y)
{
    m_angleX = 0.0f;
    m_angleY = 0.0f;
    m_moveX = 0.0f;
    m_moveY = 0.0f;
    m_zoomX = 0.0f;
    m_zoomY = 0.0f;

    //if ( (getEditMode()>=IVF_VIEW) && (getEditMode()<IVF_CREATE) )
    {
        if (Fl::event_state(FL_BUTTON3)>0) {
            //if ((getEditMode()==IVF_VIEW_ZOOM)||(getEditMode()==IVF_VIEW_PAN))
            {
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
        CIvfVec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
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
                CIvfShape* shape = m_selectedShapes->getChild(i);
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
void CIvfFltkWidget::onMotion(int x, int y)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::doPassiveMotion(int x, int y)
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

        if (m_selectedShape!=NULL)
        {
            m_selectedShape->setHighlight(CIvfShape::HS_OFF);
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
                m_selectedShape->setHighlight(CIvfShape::HS_ON);
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
        CIvfVec3d pos;
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
        CIvfVec3d pos = m_scene->getCurrentPlane()->getCursorPosition();
        pos.getComponents(wx, wy, wz);
        onCoordinate(wx, wy, wz);
        redraw();
    }

    if ((getEditMode() == IVF_CREATE_LINE)&&(m_selectEnabled))
    {
        bool needInvalidate = false;

        if (m_selectedShape!=NULL)
        {
            m_selectedShape->setHighlight(CIvfShape::HS_OFF);
            needInvalidate = true;
        }

        m_scene->pick(x, y);

        m_selectedShape = m_scene->getSelectedShape();

        if (m_selectedShape!=NULL)
        {
            bool highlight = true;
            onHighlightFilter(m_selectedShape, highlight);
            if (highlight)
            {
                m_selectedShape->setHighlight(CIvfShape::HS_ON);
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
void CIvfFltkWidget::onPassiveMotion(int x, int y)
{
}

// ------------------------------------------------------------
void CIvfFltkWidget::doMouseDown(int x, int y)
{
    // Call onMouseDown event method

    onMouseDown(x, y);
}

// ------------------------------------------------------------
void CIvfFltkWidget::onMouseDown(int x, int y)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::doMouseUp(int x, int y)
{
    // Call onMouseUp event method

    onMouseUp(x, y);
}

// ------------------------------------------------------------
void CIvfFltkWidget::doKeyboard(int key)
{
    // Call onMouseUp event method

    onKeyboard(key);
}


// ------------------------------------------------------------
void CIvfFltkWidget::onMouseUp(int x, int y)
{
}

// ------------------------------------------------------------
void CIvfFltkWidget::onCoordinate(double x, double y, double z)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onInitContext()
{
    glEnable(GL_DEPTH_TEST);

#ifndef WIN32
    glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    GLint  iMultiSample = 0;
    GLint  iNumSamples = 0;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
    glGetIntegerv(GL_SAMPLES, &iNumSamples);
#endif

    cout << "mode after context init = " << this->mode() << endl;

    m_lighting->enable();
}

// ------------------------------------------------------------
void CIvfFltkWidget::onOverlay()
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onSelect(CIvfComposite* selectedShapes)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onDeleteShape(CIvfShape *shape, bool &doit)
{
    doit = true;
}

// ------------------------------------------------------------
void CIvfFltkWidget::onCreateNode(double x, double y, double z, CIvfNode* &newNode)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onCreateLine(CIvfNode* node1, CIvfNode* node2, CIvfShape* &newLine)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onInit()
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onDestroy()
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onHighlightShape(CIvfShape *shape)
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onDeSelect()
{

}

// ------------------------------------------------------------
void CIvfFltkWidget::onMove(CIvfComposite *selectedShapes, double &dx, double &dy, double &dz, bool &doit)
{
    doit = true;
}

// ------------------------------------------------------------
void CIvfFltkWidget::onSelectFilter(CIvfShape *shape, bool &select)
{
    select = true;
}

// ------------------------------------------------------------
void CIvfFltkWidget::onHighlightFilter(CIvfShape *, bool &highlight)
{
    highlight = true;
}

// ------------------------------------------------------------
void CIvfFltkWidget::onKeyboard(int key)
{
    
}

bool CIvfFltkWidget::isInitialized()
{
    return m_initDone;
}

