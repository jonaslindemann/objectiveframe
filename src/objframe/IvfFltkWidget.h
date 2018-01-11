//
// Copyright 1999-2006 by Structural Mechanics, Lund University.
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

#ifndef _CIvfFltkWidget_h_
#define _CIvfFltkWidget_h_

// Platform depenendant includes

#include <ivf/ivfconfig.h>

#include <FL/Fl.H>
#include <FL/gl.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


// OpenGL and FLTK includes


#include <FL/Fl_Gl_Window.H>

namespace std {};
using namespace std;



#define IVF_SNAP_ON			0
#define IVF_SNAP_OFF		1

// Shape types

#define IVF_UNKNOWN			0
#define IVF_NODE			1
#define IVF_SOLID_LINE		2

// Ivf Error types

#define IVF_NO_SELECTED_NODES		1001
#define IVF_NODE_CONNECTED			1002
#define IVF_INVALID_NODE_SELECTION	1003
#define IVF_INVALID_SHAPE_SELECTION	1004
#define IVF_INVALID_SELECTION		1005
#define IVF_NO_SELECTED_SHAPES		1006

// Ivf modes

#define IVF_SELECT					0
#define IVF_SIMPLE_SELECT			500
#define IVF_VIEW					1000
#define IVF_VIEW_ZOOM				1001
#define IVF_VIEW_PAN				1002
#define IVF_CAMERA_TARGET			2010
#define IVF_MOVE					4001
#define IVF_CREATE					3000
#define IVF_CREATE_NODE				3005
#define IVF_CREATE_LINE				3006
#define IVF_CREATE_OBJECT			3500
#define IVF_MANIPULATE				5000

// Ivf manipulation modes

#define IVF_TRANS_MANIP				1000
#define IVF_ROT_MANIP				2000

#define IVF_USER_MODE				10000

// Ivf mouse modes

#define IVF_BUTTON1					0
#define IVF_BUTTON2					1
#define IVF_BUTTON3					2
#define IVF_NO_BUTTON				-1
#define IVF_SHIFT					0
#define IVF_CTRL					1
#define IVF_ALT                     2

#include <ivf/IvfBase.h>
#include <ivf/IvfComposite.h>
#include <ivf/IvfCamera.h>
#include <ivf/IvfCulledScene.h>
#include <ivf/IvfExtrusion.h>
#include <ivf/IvfNode.h>
#include <ivf/IvfShape.h>
#include <ivf/IvfBrick.h>
#include <ivf/IvfSolidLine.h>
#include <ivf/IvfWorkspace.h>

/* Later perhaps
#include <ivfmanip/IvfTranslateManipulator.h>
#include <ivfmanip/IvfRotateManipulator.h>
*/

#include <ivfmath/IvfPlane.h>
#include <ivfmath/IvfPoint3d.h>

IvfSmartPointer(CIvfFltkWidget);

/**
 * Ivf FLTK Widget
 *
 * Widget for interfacing the Ivf++ library
 * with a FLTK Application. The class should be used to derive
 * custom widgets. These widgets should implement/override
 * the onXXX methods in the class to enable it to create
 * shapes. NOTE: To derive more general widgets use the
 * CIvfFltkBase instead.
 *
 * \author Jonas Lindemann
 */
class IVFFLTK_API CIvfFltkWidget : public Fl_Gl_Window, CIvfBase
{
private:

    // State variables

    int m_moving, m_beginX, m_beginY;

    int m_currentButton;
    int m_currentState;
    int m_currentModifier;

    int m_editMode;
    int	m_clickNumber;
    int	m_snapMode;
    bool m_snapToGrid;

    int m_manipulatorMode;

    double m_workspaceSize;

    GLfloat m_angleX;
    GLfloat m_angleY;
    GLfloat m_moveX;
    GLfloat m_moveY;
    GLfloat m_zoomX;
    GLfloat m_zoomY;

    double m_startPos[3];
    double m_endPos[3];

    double m_manipStartPos[3];
    double m_manipEndPos[3];
    double m_snapUnit;

    double m_rotX, m_rotY, m_rotZ;

    CIvfPoint3d m_brick1;
    CIvfPoint3d m_brick2;

    long m_nNodes;
    long m_nLines;

    bool m_doOverlay;
    bool m_editEnabled;
    bool m_selectEnabled;
    bool m_initDone;

    double m_controlSize;

    // Interaction objects

    CIvfWorkspacePtr			m_scene;
    CIvfShapePtr				m_selectedShape;
    CIvfCameraPtr				m_camera;
    CIvfCompositePtr			m_selectedShapes;
    CIvfLightingPtr				m_lighting;
    CIvfShapePtr                m_lastShape;

protected:

    /**
     * Implements the FL_Gl_Window draw method
     *
     * This method is called when the widget is to be drawn.
     */
    void draw();

    /**
     * Implements the FL_Gl_Window handle method
     *
     * This method is called in response to GUI events.
     */
    int handle(int event);

public:
    /**
     * CIvfFltkWidget constructor
     *
     * Initializes the widget position and size and an optional title
     * @param X Widget x position.
     * @param Y Widget y position.
     * @param W Widget width.
     * @param H Widget height.
     * @param L Widget title (optional)
     */
    CIvfFltkWidget(int X, int Y, int W, int H, const char *L=0);

    /** CIvfFltkWidget destructor */
    virtual ~CIvfFltkWidget();

    IvfClassInfo("CIvfFltkWidget", CIvfBase);

    /** Returns camera used by widget */
    CIvfCamera* getCamera();

    /**
     * Add a shape to the scene.
     *
     * If addReference() is not called on the shape before adding
     * it to the scene, the shape is owned by the widget and will be
     * destroyed when not needed.
     */
    void addToScene(CIvfShape* shape);

    /**
     * Deletes selected shapes
     *
     * Shapes are deleted if not referenced.
     */
    void deleteSelected();
    void deleteSelectedKeep();

    /**
     * Manipulate selected shape
     *
     * Attaches a manipulator to the currently selected
     * shape. The current edit mode is changed to
     * \em IVF_MANIPULATE. Manipulation mode can be set
     * to two different modes:
     *
     * \param IVF_TRANS_MANIP translation manipulator
     * \param IVF_ROT_MANIP rotation manipulator
     */
    void manipulateSelected(int mode);

    /** Returns manipulation mode */
    int getManipulatorMode();

    /**
     * Creates a line between two selected nodes
     *
     * This method calls the virtual method onCreateLine(...) to
     * enable derived classes to instanciate new classes.
     */
    void createLine();

    /** Centers camera on selected shape. */
    void centerSelected();

    /**
     * Deletes all shapes in scene.
     *
     * Shapes are deleted if not referenced.
     */
    void deleteAll();

    /** Resets view to initial settings */
    void resetView();

    void clearSelection();
    void selectAll();

    /**
     * Set widget edit mode.
     *
     * Sets the edit mode of the widget. Determines how mouse actions
     * and keyboard actions should be handled.
     *
     * \param IVF_SELECT	      Selection of shapes.
     * \param IVF_VIEW_ZOOM       Rotation and zoom of current view.
     * \param IVF_VIEW_PAN        Rotation and pan of current view.
     * \param IVF_CREATE_NODE     Node creation mode.
     * \param IVF_CREATE_BOX      Box creation mode.
     * \param IVF_MOVE            Shape move mode.
     */
    void setEditMode(int mode);

    /** Returns current edit mode */
    int getEditMode();

    /**
     * Set size of workspace
     *
     * The workspace size describes the size of the grid and relative
     * node sizes and line radius. Also sets an initial snap distance
     * of the grid. Workspace is centered around the origin (0,0)
     * The visual grid spans (-size/2,-size/2)-(size/2,size/2)
     */
    void setWorkspace(double size);

    /** Returns workspace size */
    double getWorkspace();

    /** Return the scene object used by the widget */
    CIvfWorkspace* getScene();

    /** Return overlay use */
    bool getUseOverlay();

    /** Enables the use of a overlay layer (see onOverlay) */
    void setUseOverlay(bool flag);

    /** Enables/disables selection */
    void setSelectEnable(bool flag);

    /** Enables/disables edit */
    void setEditEnabled(bool flag);

    /** Enables/disables grid snap */
    void setSnapToGrid(bool flag);

    /** Return snap to grid state */
    bool getSnapToGrid();

    /** Return current mouse button */
    int getCurrentMouseButton();
    int getCurrentModifier();

    CIvfComposite* getSelectedShapes();

    bool isInitialized();

protected:

    // Internal event handlers

    virtual void doMouseUp(int x, int y);       // Calls onMouseUp
    virtual void doMouseDown(int x, int y);     // Calls onMouseDown
    virtual void doPassiveMotion(int x, int y); // Calls onPassiveMotion
    virtual void doMotion(int x, int y);        // Calls onMotion
    virtual void doMouse(int x, int y);         // Calls onMouse
    virtual void doKeyboard(int key);

    /**
     * onInit event
     *
     * onInit is called the first time an OpenGL context is
     * activated. All OpenGL dependent intialization code
     * should be placed here, including Ivf++ code.
     */
    virtual void onInit();

    /**
     * onDestroy event
     *
     * This method is called when the widget is destroyed.
     */
    virtual void onDestroy();

    /**
     * onCreateNode event
     *
     * This method is called if a node is to be created.
     * The following code sample shows how a onCreateNode method can
     * be implemented.
     * \code
     * void CIvfMyWidget::onCreateNode(double x, double y, double z, CIvfNode* &newNode)
     * {
     * 	  newNode = new CIvfNode();
     *	  newNode->setPosition(x, y, z);
     * 	  newNode->setNodeSize(m_nodeSize);
     * 	  newNode->setNodeType(m_nodeType);
     * 	  newNode->setMaterial(m_nodeMaterial);
     * }
     * \endcode
     * @param x node x coordinate
     * @param y node y coordinate
     * @param z node z coordinate
     * @param newNode This parameter shoud be assigned if a new node has been created.
     *
     */
    virtual void onCreateNode(double x, double y, double z, CIvfNode* &newNode);

    /**
     * onCreateLine event
     *
     * This method is called if a line is to created.
     * The following code shows how a onCreateLine method can be
     * implemented.
     * \code
     * void CIvfMyWidget::onCreateLine(CIvfNode* node1, CIvfNode* node2, CIvfShape* &newLine)
     * {
     * 	  CIvfSolidLine* line = new CIvfSolidLine();
     * 	  line->setNodes(node1, node2);
     * 	  line->setRadius(m_lineRadius);
     * 	  line->setSides(m_lineSides);
     * 	  line->setMaterial(m_lineMaterial);
     *    newLine = (CIvfShape*) line;
     * }
     * \endcode
     * @param node1 First selected node
     * @param node2 Second selected node
     * @param newLine This parameter should be assigned if a new line has been created.
     */
    virtual void onCreateLine(CIvfNode* node1, CIvfNode* node2, CIvfShape* &newLine);

    /**
     * onSelect event
     *
     * This method is called when a selection has occured.
     * @param selectedShapes Currently selected shapes.
     */
    virtual void onSelect(CIvfComposite* selectedShapes);

    /**
     * onDeSelect event
     *
     * This method is called when objects have be deselected.
     */
    virtual void onDeSelect();
    /**
     * onCoordinate event
     *
     * This method is called when the cursor is updated.
     * @param x Cursor x coordinate
     * @param y Cursor y coordinate
     * @param z Cursor z coordinate
     */
    virtual void onCoordinate(double x, double y, double z);

    /**
     * onDeleteShape event
     *
     * Is called when an object is to be deleted. The \em doit
     * variable determines if the object can be deleted. If
     * \em doit is \em true the object will be deleted.
     */
    virtual void onDeleteShape(CIvfShape* shape, bool &doit);

    /**
     * onInitContext event
     *
     * This event is called when an OpenGL context is initialized.
     * OpenGL state initialization code should be placed here.
     */
    virtual void onInitContext();

    /**
     * onOverlay event
     *
     * This event is called when an overlay is to be drawn. For this
     * method to be called, the UseOverlay property must be set using
     * the setUseOverlay method. The view port is configured for
     * 2D drawing when this method is called.
     */
    virtual void onOverlay();

    /**
     * onHighlightShape event
     *
     * This event is called when an object has been highlighted in
     * a select operation.
     */
    virtual void onHighlightShape(CIvfShape* shape);

    /**
     * onMove event
     *
     * This event is called in response to a move operation.
     * The dx, dy, dz values contains the requested move distance. These
     * can be modified to control the move behavior. The doit flag
     * must be set to true if the move operation should be performed or
     * not.
     */
    virtual void onMove(CIvfComposite* selectedShapes, double &dx, double &dy, double &dz, bool &doit);

    virtual void onSelectFilter(CIvfShape* shape, bool &select);
    virtual void onHighlightFilter(CIvfShape*, bool &highlight);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);
    virtual void onMouse(int x, int y);
    virtual void onMouseDown(int x, int y);
    virtual void onMouseUp(int x, int y);
    virtual void onKeyboard(int key);
};

#endif

