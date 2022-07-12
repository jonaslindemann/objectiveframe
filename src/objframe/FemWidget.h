#pragma once

#define OBJFRAME_VERSION_STRING "ObjectiveFrame 2.0.0"
#define ADVANCED_GL
#define USE_IMGUI

#include "ObjframeConfig.h"

#include <string>
#include <sstream>

#include <ivf/Base.h>
#include <ivf/Shape.h>
#include <ivf/Material.h>
#include <ivf/QuadPlane.h>
#include <ivf/Sphere.h>
#include <ivf/SelectOrtho.h>
#include <ivf/Composite.h>
#include <ivf/ExtrArrow.h>

#include <VisFemNode.h>
#include <VisFemBeam.h>
#include <VisBeamModel.h>
#include <VisFemBeamLoad.h>
#include <VisFemNodeLoad.h>
#include <VisFemNodeBC.h>

#include <FemBeamLoad.h>
#include <FemBeamNodeLoad.h>
#include <FemBeamNodeBC.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#include <coord_window.h>
#include <node_prop_window.h>
#include <new_model_popup.h>
#include <message_popup.h>
#include <node_bcs_window.h>
#include <bc_prop_popup.h>
#include <node_loads_window.h>
#include <node_load_prop_popup.h>
#include <settings_window.h>
#include <element_loads_window.h>
#include <materials_window.h>

#include "Area2D.h"
#include "PlaneButton.h"
#include "ButtonGroup.h"

#include "FemInternalSolver.h"

#include "BeamPropDlg.h"
#include "MaterialsDlg.h"
#include "ElementLoadsDlg.h"

enum class RepresentationMode {
	Fem,
	Geometry,
	Displacements,
	Results
};

enum class CustomMode {
	Normal,
	Feedback
};

enum class SelectMode {
	All,
	Nodes,
	Elements,
	GroundNodes
};

enum class DeleteMode {
	All,
	Nodes,
	Elements
};

enum class HighlightMode {
	All,
	Nodes,
	Elements
};

enum ToolbarButton {
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

class FemWidget : public FltkWidget {
private:
	std::string m_coordText;
	std::string m_modeDescr;
	std::string m_xCoord;
	std::string m_yCoord;
	std::string m_zCoord;

	RepresentationMode m_representation;

	std::string   m_fileName;
	std::string   m_progPath;

	bool    m_overlaySelected;
	bool    m_haveScaleFactor;
	bool    m_needRecalc;
	bool    m_overWorkspace;
	bool    m_lastOverWorkspace;
	bool    m_lockScaleFactor;

	CustomMode m_customMode;
	bool    m_customModeSet;
	double  m_alfa;
	double  m_beta;
	double  m_startAlfa;
	double  m_startBeta;

	int     m_mouseDownPos[2];

	SelectMode m_selectFilter;
	DeleteMode m_deleteFilter;
	HighlightMode m_highlightFilter;

	int		m_width;
	int		m_height;

	double  m_relNodeSize;
	double  m_relLoadSize;
	double  m_relLineRadius;

	bool m_saneModel;

	int m_argc;
	char** m_argv;

	double m_tactileForceValue;

	Fl_Widget* m_coordWidget;

	CFemBeamMaterialPtr m_currentMaterial;
	CFemBeamLoadPtr     m_currentElementLoad;
	CFemBeamNodeLoadPtr m_currentNodeLoad;
	CFemBeamNodeBCPtr   m_currentNodeBC;
	VisFemNodePtr       m_interactionNode;
#ifdef USE_LEAP
	LeapInteraction* m_leapinteraction;
#endif

	CFemInternalSolver*  m_internalSolver;

	ivf::MaterialPtr     m_nodeMaterial;
	ivf::MaterialPtr	 m_lineMaterial;
	ivf::ShapePtr        m_selectedShape;
#ifdef ADVANCED_GL
	CIvfPlaneButton* m_selectedButton;
#endif
	ivf::CompositePtr    m_beamLoads;
	VisBeamModelPtr	     m_beamModel;
	ivf::QuadPlanePtr    m_plane;
	ivf::SpherePtr       m_sphere;
	ivf::ExtrArrowPtr    m_tactileForce;

	ivf::SpherePtr       m_nodeCursor;

	// Overlay stuff

	vector<CIvfArea2D*> m_areas;
	vector<CIvfPlaneButton*> m_buttons;
	ivf::SelectOrthoPtr m_overlayScene;
	CIvfButtonGroup* m_editButtons;
	CIvfButtonGroup* m_viewButtons;
	CIvfButtonGroup* m_objectButtons;
	CIvfArea2D* m_editArea;
	CIvfArea2D* m_objectArea;
	CIvfArea2D* m_viewArea;

	float m_hintColor[3];
	bool m_hintFinished;

	// Result visualisation

	// Private methods

	void doMouse(int x, int y);
	void setupOverlay();

	// Dialogs

	BeamPropDlg* m_dlgBeamProp;
	MaterialsDlg* m_dlgMaterials;
	ElementLoadsDlg* m_dlgElementLoads;
	//StructureDlg* m_dlgStructure;

	bool m_showDemoWindow;

	CoordWindowPtr m_coordWindow;
	NodePropWindowPtr m_nodePropWindow;
	NewModelPopupPtr m_newModelPopup;
	MessagePopupPtr m_messagePopup;
	NodeBCsWindowPtr m_nodeBCsWindow;
	BCPropPopupPtr m_bcPropPopup;
	NodeLoadsWindowPtr m_nodeLoadsWindow;
	SettingsWindowPtr m_settingsWindow;
	ElementLoadsWindowPtr m_elementLoadsWindow;
	MaterialsWindowPtr m_materialsWindow;

	bool m_showStyleEditor;
	bool m_showMetricsWindow;
	bool m_showNewFileDlg;
	bool m_showNodeBCsWindow;
	bool m_showBCPropPopup;
	bool m_showNodeLoadssWindow;
	bool m_showLoadPropPopup;

	void makeToolWindow(Fl_Window* window);
	std::string float2str(double value);

public:
	FemWidget(int X, int Y, int W, int H, const char* L = 0);
	virtual ~FemWidget();

	// Get set methods
	void setCoordWidget(Fl_Widget* widget);
	void setFileName(const std::string& name);
	void setCurrentMaterial(CFemBeamMaterial* material);
	void setCurrentBeamLoad(CFemBeamLoad* elementLoad);
	ivf::Shape* getSelectedShape();
	CFemBeamMaterial* getCurrentMaterial();
	Fl_Widget* getCoordWidget();
	const std::string getFileName();
	void setSelectFilter(SelectMode filter);
	void setBeamRefreshMode(int mode);
	void setEditMode(WidgetMode mode);
	void setCustomMode(CustomMode mode);
	void setScalefactor(double scalefactor);
	double getScalefactor();
	void setArguments(int argc, char** argv);
	void setCurrentNodeBC(CFemBeamNodeBC* bc);
	void setRotationSelected(double rotation);
	void setCurrentNodeLoad(CFemBeamNodeLoad* nodeLoad);
	CFemBeamNodeLoad* getCurrentNodeLoad();
	CFemBeamLoad* getCurrentBeamLoad();
	void setProgramPath(const std::string& progPath);
	const std::string getProgPath();
	void setResultType(int type);
	CFemBeamNodeBC* getCurrentNodeBC();
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
	void removeMaterialFromSelected();
	void assignMaterialToSelected();
	void newModel();
	void showBeamLoads();
	void addBeamLoad(CFemBeamLoad* elementLoad);
	void deleteBeamLoad(CFemBeamLoad* elementLoad);
	void assignBeamLoadSelected();
	void selectAllElements();
	void selectAllNodes();
	void showMaterials();
	void showProperties();
	void executeCalc();
	void deleteNodeBC(CFemBeamNodeBC* bc);
	void addNodeBC(CFemBeamNodeBC* bc);
	void assignNodeBCSelected();
	void assignNodeFixedBCSelected();
	void assignNodePosBCSelected();
	void assignNodeFixedBCGround();
	void assignNodePosBCGround();
	void deleteNodeLoad(CFemBeamNodeLoad* nodeLoad);
	void assignNodeLoadSelected();
	void addNodeLoad(CFemBeamNodeLoad* nodeLoad);
	void doFeedback();

	void showMessage(std::string message);

	ivf::ExtrArrowPtr getTactileForce();
	void setTactileForce(ivf::ExtrArrowPtr force);
	VisFemNodePtr getInteractionNode();
	void setInteractionNode(VisFemNode* interactionNode);

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
	void onButton(int objectName, CIvfPlaneButton* button);

	virtual void onDrawImGui();
	virtual void onInitImGui();

	virtual void onInit();
	virtual void onInitContext();
	virtual void onOverlay();
};

