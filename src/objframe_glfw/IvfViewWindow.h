#pragma once

#include <map>
#include <memory>
#include <string>

#include <GLFWWindow.h>

// Widget modes

enum class WidgetMode {
    Select,
    SimpleSelect,
    View,
    ViewZoom,
    ViewPan,
    CameraTarget,
    Move,
    Create,
    CreateNode,
    CreateLine,
    CreateObject,
    SelectPosition,
    SelectVolume,
    BoxSelection,
    Manipulate,
    User
};

// Mouse modes

enum class ButtonState {
    bsButton1,
    bsButton2,
    bsButton3,
    bsNoButton,
    bsShift,
    bsCtrl,
    bsAlt
};

#include <ivf/Base.h>
#include <ivf/Brick.h>
#include <ivf/Camera.h>
#include <ivf/Composite.h>
#include <ivf/CulledScene.h>
#include <ivf/Extrusion.h>
#include <ivf/Node.h>
#include <ivf/Shape.h>
#include <ivf/SolidLine.h>
#include <ivf/Workspace.h>

#include <ivfmath/Plane.h>
#include <ivfmath/Point3d.h>

#include <ofmath/grid_plane.h>

enum ModifierKey {
    mkShift,
    mkCtrl,
    mkAlt,
    mkNone
};

class IvfViewWindow : public GLFWWindow {
private:
    // State variables

    int m_moving, m_beginX, m_beginY;

    ButtonState m_currentButton;
    ButtonState m_currentModifier;

    WidgetMode m_editMode;
    int m_clickNumber;
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

    double m_volumeStart[3];
    double m_volumeEnd[3];

    double m_manipStartPos[3];
    double m_manipEndPos[3];
    double m_snapUnit;

    double m_rotX, m_rotY, m_rotZ;

    ivf::Point3d m_brick1;
    ivf::Point3d m_brick2;

    long m_nNodes;
    long m_nLines;

    bool m_doOverlay;
    bool m_doUnderlay;
    bool m_editEnabled;
    bool m_selectEnabled;
    bool m_initDone;
    bool m_disableRedrawTimer;
    bool m_quit;
    bool m_moveStart;

    bool m_mouseUpdate;

    bool m_initialised;

    double m_controlSize;

    bool m_offScreenRendering;

    bool m_customPick;

    GLuint m_screenTexture;
    GLuint m_multiFbo, m_stdFbo, m_colorBuffer, m_depthBuffer;

    int m_prevWindowSize[2];

    // Interaction objects

    ivf::WorkspacePtr m_scene;
    ivf::ShapePtr m_selectedShape;
    ivf::CameraPtr m_camera;
    ivf::CompositePtr m_selectedShapes;
    ivf::LightingPtr m_lighting;
    ivf::ShapePtr m_lastShape;
    ivf::WireBrickPtr m_volumeSelection;

    ofmath::GridPlane m_xzPlane;
    ofmath::GridPlane m_xyPlane;
    ofmath::GridPlane m_yzPlane;

public:
    IvfViewWindow(int width, int height, const std::string title, GLFWmonitor *monitor = nullptr,
                  GLFWwindow *shared = nullptr);
    virtual ~IvfViewWindow();

    static std::shared_ptr<IvfViewWindow> create(int width, int height, const std::string title,
                                                 GLFWmonitor *monitor = nullptr, GLFWwindow *shared = nullptr);

public:
    virtual void onGlfwKey(int key, int scancode, int action, int mods) override;
    virtual void onGlfwMousePosition(double x, double y) override;
    virtual void onGlfwMouseButton(int button, int action, int mods) override;
    virtual void onGlfwResize(int width, int height) override;
    virtual void onGlfwDraw() override;

public:
    void redraw();
    void quit();

    bool isOverWindow();

    /** Returns camera used by widget */
    ivf::Camera *getCamera();

    void updateCursor(int x, int y);

    /**
     * Add a shape to the scene.
     *
     * If addReference() is not called on the shape before adding
     * it to the scene, the shape is owned by the widget and will be
     * destroyed when not needed.
     */
    void addToScene(ivf::Shape *shape);

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
    // void manipulateSelected(int mode);

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

    void addSelection(ivf::Shape *shape);

    bool selectionContains(std::string shapeName);

    int selectionShapeCount(std::string shapeName);

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
    void setEditMode(WidgetMode mode);

    /** Returns current edit mode */
    WidgetMode getEditMode();

    /**
     * Set size of workspace
     *
     * The workspace size describes the size of the grid and relative
     * node sizes and line radius. Also sets an initial snap distance
     * of the grid. Workspace is centered around the origin (0,0)
     * The visual grid spans (-size/2,-size/2)-(size/2,size/2)
     */
    void setWorkspace(double size, bool resetCamera = true);

    /** Returns workspace size */
    double getWorkspace();

    /** Return the scene object used by the widget */
    ivf::Workspace *getScene();

    /** Return overlay use */
    bool getUseOverlay();

    /** Enables the use of a overlay layer (see onOverlay) */
    void setUseOverlay(bool flag);

    void setUseUnderlay(bool flag);

    bool useUnderlay();

    /** Enables/disables selection */
    void setSelectEnable(bool flag);

    /** Enables/disables edit */
    void setEditEnabled(bool flag);

    /** Enables/disables grid snap */
    void setSnapToGrid(bool flag);

    /** Return snap to grid state */
    bool getSnapToGrid();

    /** Return current mouse button */
    ButtonState getCurrentMouseButton();
    ButtonState getCurrentModifier();

    ivf::Composite *getSelectedShapes();

    void selectAllBox();
    bool isInsideVolume(ivf::Shape *shape);

    void setUseCustomPick(bool flag);
    bool useCustomPick();

protected:
    // Internal event handlers

    virtual void doMouseUp(int x, int y);       // Calls onMouseUp
    virtual void doMouseDown(int x, int y);     // Calls onMouseDown
    virtual void doPassiveMotion(int x, int y); // Calls onPassiveMotion
    virtual void doMotion(int x, int y);        // Calls onMotion
    virtual void doMouse(int x, int y);         // Calls onMouse
    virtual void doKeyboard(int key);
    virtual void doKeyboardReleased(int key);
    virtual void doShortcut(ModifierKey modifier, int key);

    void doInitImGui();
    void doDrawImGui();

public:
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

    virtual void onShortcut(ModifierKey modifier, int key);

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
    virtual void onCreateNode(double x, double y, double z, ivf::Node *&newNode);

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
    virtual void onCreateLine(ivf::Node *node1, ivf::Node *node2, ivf::Shape *&newLine);

    /**
     * onSelect event
     *
     * This method is called when a selection has occured.
     * @param selectedShapes Currently selected shapes.
     */
    virtual void onSelect(ivf::Composite *selectedShapes);

    virtual bool onInsideVolume(ivf::Shape *shape);

    virtual void onSelectPosition(double x, double y, double z);

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
    virtual void onDeleteShape(ivf::Shape *shape, bool &doit);

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

    virtual void onUnderlay();

    /**
     * onHighlightShape event
     *
     * This event is called when an object has been highlighted in
     * a select operation.
     */
    virtual void onHighlightShape(ivf::Shape *shape);

    /**
     * onMove event
     *
     * This event is called in response to a move operation.
     * The dx, dy, dz values contains the requested move distance. These
     * can be modified to control the move behavior. The doit flag
     * must be set to true if the move operation should be performed or
     * not.
     */
    virtual void onMove(ivf::Composite *selectedShapes, double &dx, double &dy, double &dz, bool &doit);

    virtual void onMoveCompleted();

    virtual void onSelectFilter(ivf::Shape *shape, bool &select);
    virtual void onHighlightFilter(ivf::Shape *, bool &highlight);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);
    virtual void onMouse(int x, int y);
    virtual void onMouseDown(int x, int y);
    virtual void onMouseUp(int x, int y);
    virtual void onKeyboard(int key);
    virtual void onSelectVolume(double x0, double y0, double z0, double x1, double y1, double yz);
    virtual ivf::Shape *onPick(int x, int y);

    virtual void onPostRender();
    virtual void onPreRender();

    virtual void onInitImGui();
    virtual void onDrawImGui();
};

typedef std::shared_ptr<IvfViewWindow> IvfViewWindowPtr;
