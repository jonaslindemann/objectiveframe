#ifndef _CDialogMgr_h_
#define _CDialogMgr_h_

#include "NodePropDlg.h"
#include "BeamPropDlg.h"
#include "MaterialsDlg.h"
#include "ElementLoadsDlg.h"
#include "NodeLoadsDlg.h"
#include "NodeBCsDlg.h"
#include "ScalefactorDlg.h"
#include "StructureDlg.h"

#include "FemBeamLoad.h"
#include "FemBeamLoad.h"
#include "FemBeamNodeLoad.h"
#include "FemBeamNodeBC.h"

class CDialogMgr {
private:
	void* m_widget;
	CNodePropDlg* m_dlgNodeProp;
	CBeamPropDlg* m_dlgBeamProp;
	CMaterialsDlg* m_dlgMaterials;
	CElementLoadsDlg* m_dlgElementLoads;
	CNodeLoadsDlg* m_dlgNodeLoads;
	CNodeBCsDlg* m_dlgNodeBCs;
	CScalefactorDlg* m_dlgScalefactor;
	CStructureDlg* m_dlgStructure;
public:
	void removeBeamLoadsFromSelected();
	void removeNodeBCsFromSelected();
	void removeNodeLoadsFromSelected();
	void setNeedRecalc(bool flag);
	CStructureDlg* getStructureDlg();
	CScalefactorDlg* getScalefactorDlg();
	double getScalefactor();
	void setScalefactor(double scalefactor);
	void assignNodeBCSelected();
	void deleteNodeBC(CFemBeamNodeBC* bc);
	void addNodeBC(CFemBeamNodeBC* bc);
	CNodeBCsDlg* getNodeBCsDlg();
	void setNodeBCsDlg(CNodeBCsDlg *dlg);
	void setCurrentNodeBC(CFemBeamNodeBC* bc);
	void setRotationSelected(double rotation);
	void setNodeLoadsDlg(CNodeLoadsDlg* dlg);
	CNodeLoadsDlg* getNodeLoadsDlg();
	CDialogMgr();
	virtual ~CDialogMgr();

	// Methods

	void removeMaterialFromSelected();
	void assignMaterialToSelected();
	void addBeamLoad(CFemBeamLoad* elementLoad);
	void deleteBeamLoad(CFemBeamLoad* elementLoad);
	void assignBeamLoadSelected();
	void hideAll();
	void deleteNodeLoad(CFemBeamNodeLoad* nodeLoad);
	void assignNodeLoadSelected();
	void addNodeLoad(CFemBeamNodeLoad* nodeLoad);

	// Get/set methods

	void setElementLoadsDlg(CElementLoadsDlg* dlg);
	void setMaterialsDlg(CMaterialsDlg* dlg);
	void setNodePropDlg(CNodePropDlg* dlg);
	void setBeamPropDlg(CBeamPropDlg* dlg);
	void setWidget(void* widget);
	void setCurrentNodeLoad(CFemBeamNodeLoad* nodeLoad);
	void setCurrentBeamLoad(CFemBeamLoad* elementLoad);
	CMaterialsDlg* getMaterialsDlg();
	CBeamPropDlg* getBeamPropDlg();
	CNodePropDlg* getNodePropDlg();
	CElementLoadsDlg* getElementLoadsDlg();
	CStructureDlg* getStrucrureDlg();

};

#endif 
