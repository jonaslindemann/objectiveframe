#pragma once

#define OBJFRAME_VERSION_STRING "ObjectiveFrame 2"
#define OBJFRAME_RELEASE "Release version - 2.0.0"
#define OBJFRAME_COPYRIGHT_STRING "Copyright (C) 2001-2022\nDivision of Structural Mechanics, Lund university"
#define OBJFRAME_AUTHOR1 "Main author: Jonas Lindemann"
#define OBJFRAME_AUTHOR2 "Contributors: Pierre Olsson, Daniel Akesson"

#define ADVANCED_GL
#define USE_IMGUI

#include "ObjframeConfig.h"

#include <sstream>
#include <string>

#include <ivf/Base.h>
#include <ivf/Billboard.h>
#include <ivf/BitmapFont.h>
#include <ivf/Composite.h>
#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>
#include <ivf/QuadPlane.h>
#include <ivf/SelectOrtho.h>
#include <ivf/Shape.h>
#include <ivf/Sphere.h>

#include <vfem/beam.h>
#include <vfem/beam_load.h>
#include <vfem/beam_model.h>
#include <vfem/node.h>
#include <vfem/node_bc.h>
#include <vfem/node_load.h>

#include <ofem/beam_load.h>
#include <ofem/beam_node_bc.h>
#include <ofem/beam_node_load.h>
#include <ofem/calfem_writer.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#include <ofui/bc_prop_popup.h>
#include <ofui/console_window.h>
#include <ofui/coord_window.h>
#include <ofui/element_loads_window.h>
#include <ofui/element_prop_window.h>
#include <ofui/log_window.h>
#include <ofui/materials_window.h>
#include <ofui/message_popup.h>
#include <ofui/new_model_popup.h>
#include <ofui/node_bcs_window.h>
#include <ofui/node_load_prop_popup.h>
#include <ofui/node_loads_window.h>
#include <ofui/node_prop_window.h>
#include <ofui/settings_window.h>

#include "Area2D.h"
#include "ButtonGroup.h"
#include "PlaneButton.h"

#include "FemInternalSolver.h"

enum class RepresentationMode
{
    Fem,
    Geometry,
    Displacements,
    Results
};

enum class CustomMode
{
    Normal,
    Feedback
};

enum class SelectMode
{
    All,
    Nodes,
    Elements,
    GroundNodes
};

enum class DeleteMode
{
    All,
    Nodes,
    Elements
};

enum class HighlightMode
{
    All,
    Nodes,
    Elements
};

enum ToolbarButton
{
    Select,
    Move,
    CreateNode,
    CreateBeam,
    Delete,
    Inspect,
    NodeBC,
    NodeLoad,
    BeamLoad,
    ViewZoom,
    ViewPan,
    ViewReset,
    ViewCenter,
    Materials,
    Feedback
};

template <typename T>
string to_string(T Number)
{
    ostringstream ss;
    ss << Number;
    return ss.str();
}

#include "FltkWidget.h"

class FemWidget : public FltkWidget
{
private:
    std::string m_coordText;
    std::string m_modeDescr;
    std::string m_xCoord;
    std::string m_yCoord;
    std::string m_zCoord;

    RepresentationMode m_representation;

    std::string m_fileName;
    std::string m_progPath;

    bool m_overlaySelected;
    bool m_haveScaleFactor;
    bool m_needRecalc;
    bool m_overWorkspace;
    bool m_lastOverWorkspace;
    bool m_lockScaleFactor;
    bool m_nodeSelection;
    bool m_singleNodeSelection;
    bool m_elementSelection;
    bool m_singleElementSelection;
    bool m_mixedSelection;

    CustomMode m_customMode;
    bool m_customModeSet;
    double m_alfa;
    double m_beta;
    double m_startAlfa;
    double m_startBeta;

    int m_mouseDownPos[2];

    SelectMode m_selectFilter;
    DeleteMode m_deleteFilter;
    HighlightMode m_highlightFilter;

    int m_width;
    int m_height;

    double m_relNodeSize;
    double m_relLoadSize;
    double m_relLineRadius;

    bool m_saneModel;

    int m_argc;
    char** m_argv;

    double m_tactileForceValue;

    Fl_Widget* m_coordWidget;

    ofem::BeamMaterialPtr m_currentMaterial;
    ofem::BeamLoadPtr m_currentElementLoad;
    ofem::BeamNodeLoadPtr m_currentNodeLoad;
    ofem::BeamNodeBCPtr m_currentNodeBC;
    vfem::NodePtr m_interactionNode;
#ifdef USE_LEAP
    LeapInteraction* m_leapinteraction;
#endif

    FrameSolver* m_internalSolver;

    ivf::MaterialPtr m_nodeMaterial;
    ivf::MaterialPtr m_lineMaterial;
    ivf::ShapePtr m_selectedShape;
#ifdef ADVANCED_GL
    PlaneButton* m_selectedButton;
#endif
    ivf::CompositePtr m_beamLoads;
    vfem::BeamModelPtr m_beamModel;
    ivf::QuadPlanePtr m_plane;
    ivf::SpherePtr m_sphere;
    ivf::ExtrArrowPtr m_tactileForce;

    ivf::SpherePtr m_nodeCursor;

    ivf::CompositePtr m_textLayer;
    ivf::BitmapFontPtr m_labelFont;
    ivf::BitmapFontPtr m_axisFont;
    ivf::BitmapFontPtr m_redFont;
    ivf::BitmapFontPtr m_greenFont;

    ivf::BillBoardPtr m_background;

    // Overlay stuff

    vector<Area2D*> m_areas;
    vector<PlaneButton*> m_buttons;
    ivf::SelectOrthoPtr m_overlayScene;
    ButtonGroup* m_editButtons;
    // ButtonGroup* m_viewButtons;
    ButtonGroup* m_objectButtons;
    Area2D* m_editArea;
    Area2D* m_objectArea;
    // Area2D* m_viewArea;

    PlaneButton* m_prevButton;

    // float m_hintColor[3];
    bool m_hintFinished;

    // Result visualisation

    // Private methods

    void doMouse(int x, int y);
    void setupOverlay();

    // Dialogs

    // bool m_showDemoWindow;

    ofui::CoordWindowPtr m_coordWindow;
    ofui::NodePropWindowPtr m_nodePropWindow;
    ofui::NewModelPopupPtr m_newModelPopup;
    ofui::MessagePopupPtr m_messagePopup;
    ofui::NodeBCsWindowPtr m_nodeBCsWindow;
    ofui::BCPropPopupPtr m_bcPropPopup;
    ofui::NodeLoadsWindowPtr m_nodeLoadsWindow;
    ofui::SettingsWindowPtr m_settingsWindow;
    ofui::ElementLoadsWindowPtr m_elementLoadsWindow;
    ofui::MaterialsWindowPtr m_materialsWindow;
    ofui::ElementPropWindowPtr m_elementPropWindow;
    ofui::LogWindowPtr m_logWindow;
    ofui::ConsoleWindowPtr m_consoleWindow;

    bool m_showStyleEditor;
    bool m_showMetricsWindow;
    bool m_showNewFileDlg;
    bool m_showNodeBCsWindow;
    bool m_showBCPropPopup;
    // bool m_showNodeLoadssWindow;
    // bool m_showLoadPropPopup;

    std::string float2str(double value);

    void log(std::string message);
    void onMessage(std::string message);

    void console(std::string message);

public:
    FemWidget(int X, int Y, int W, int H, const char* L = 0);
    virtual ~FemWidget();

    // Get set methods
    void setCoordWidget(Fl_Widget* widget);
    void setFileName(const std::string& name);
    void setCurrentMaterial(ofem::BeamMaterial* material);
    void setCurrentBeamLoad(ofem::BeamLoad* elementLoad);
    ivf::Shape* getSelectedShape();
    ofem::BeamMaterial* getCurrentMaterial();
    Fl_Widget* getCoordWidget();
    const std::string getFileName();
    void setSelectFilter(SelectMode filter);
    void setBeamRefreshMode(int mode);
    void setEditMode(WidgetMode mode);
    void setCustomMode(CustomMode mode);
    void setScalefactor(double scalefactor);
    double getScalefactor();
    void setArguments(int argc, char** argv);
    void setCurrentNodeBC(ofem::BeamNodeBC* bc);
    void setRotationSelected(double rotation);
    void setCurrentNodeLoad(ofem::BeamNodeLoad* nodeLoad);
    ofem::BeamNodeLoad* getCurrentNodeLoad();
    ofem::BeamLoad* getCurrentBeamLoad();
    void setProgramPath(const std::string& progPath);
    const std::string getProgPath();
    void setResultType(int type);
    ofem::BeamNodeBC* getCurrentNodeBC();
    void setRelLoadSize(double size);
    double getRelLoadSize();
    void setRelLineRadius(double radius);
    double getRelLineRadius();
    void setRelNodeSize(double size);
    double getRelNodeSize();
    void setNeedRecalc(bool flag);
    void setHighlightFilter(HighlightMode filter);
    void setDeleteFilter(DeleteMode filter);
    void setRepresentation(RepresentationMode repr);

    ofem::BeamModel* getModel();

    // Methods

    void hideAllDialogs();
    void removeBeamLoadsFromSelected();
    void removeNodeBCsFromSelected();
    void removeNodeLoadsFromSelected();
    void removeNodesFromNodeLoad();
    void removeBCsFromBC();
    void refreshToolbars();
    void onHighlightFilter(ivf::Shape* shape, bool& highlight);
    void deleteSelected();
    void unlockScaleFactor();
    void lockScaleFactor();

    virtual void setWorkspace(double size, bool resetCamera = true);
    void open();
    void save();
    void saveAs();
    void exportAsCalfem();
    void removeMaterialFromSelected();
    void assignMaterialToSelected();
    void newModel();
    void showBeamLoads();
    void addBeamLoad(ofem::BeamLoad* elementLoad);
    void deleteBeamLoad(ofem::BeamLoad* elementLoad);
    void assignBeamLoadSelected();
    void selectAllElements();
    void selectAllNodes();
    void showMaterials();
    void showProperties();
    void executeCalc();
    void deleteNodeBC(ofem::BeamNodeBC* bc);
    void addNodeBC(ofem::BeamNodeBC* bc);
    void assignNodeBCSelected();
    void assignNodeFixedBCSelected();
    void assignNodePosBCSelected();
    void assignNodeFixedBCGround();
    void assignNodePosBCGround();
    void deleteNodeLoad(ofem::BeamNodeLoad* nodeLoad);
    void assignNodeLoadSelected();
    void addNodeLoad(ofem::BeamNodeLoad* nodeLoad);
    void doFeedback();
    vfem::Node* addNode(double x, double y, double z);

    void showMessage(std::string message);

    ivf::ExtrArrowPtr getTactileForce();
    void setTactileForce(ivf::ExtrArrowPtr force);
    vfem::NodePtr getInteractionNode();
    void setInteractionNode(vfem::Node* interactionNode);
    void updateAxisLabels();

    // Implemented widget events

    void onCreateNode(double x, double y, double z, ivf::Node*& newNode);
    void onCreateLine(ivf::Node* node1, ivf::Node* node2, ivf::Shape*& newLine);
    void onSelect(ivf::Composite* selectedShapes);
    void onCoordinate(double x, double y, double z);
    void onDeleteShape(ivf::Shape* shape, bool& doit);
    void onHighlightShape(ivf::Shape* shape);
    void onMouse(int x, int y);
    void onMouseDown(int x, int y);
    void onMouseUp(int x, int y);
    void onPassiveMotion(int x, int y);
    void onSelectFilter(ivf::Shape* shape, bool& select);
    void onMove(ivf::Composite* selectedShapes, double& dx, double& dy, double& dz, bool& doit);
    void onMotion(int x, int y);
    void onDeSelect();
    void onKeyboard(int key);
    void onButton(int objectName, PlaneButton* button);
    void onOverButton(int objectName, PlaneButton* button);
    void onShortcut(ModifierKey modifier, int key);

    virtual void onDrawImGui();
    virtual void onInitImGui();

    virtual void onInit();
    virtual void onInitContext();
    virtual void onOverlay();
    virtual void onUnderlay();
    virtual void onPostRender();
};
