#include "DialogMgr.h"

#include "IvfFemWidget.h"

// ------------------------------------------------------------
CDialogMgr::CDialogMgr()
{
	cout << "DialogMgr: Creating node prop dialog." << endl;
	m_dlgNodeProp = new CNodePropDlg();
	
	cout << "DialogMgr: Creating beam prop dialog." << endl;
	m_dlgBeamProp = new CBeamPropDlg();
	m_dlgBeamProp->setDialogMgr(this);

	cout << "DialogMgr: Creating materials dialog." << endl;
	m_dlgMaterials = new CMaterialsDlg();
	cout << "DialogMgr: Setting dialog manager." << endl;
	m_dlgMaterials->setDialogMgr(this);
	
	cout << "DialogMgr: Creating element load dialog." << endl;
	m_dlgElementLoads = new CElementLoadsDlg();
	m_dlgElementLoads->setDialogMgr(this);

	cout << "DialogMgr: Creating node load dialog." << endl;
	m_dlgNodeLoads = new CNodeLoadsDlg();
	m_dlgNodeLoads->setDialogMgr(this);

	cout << "DialogMgr: Creating node bc dialog." << endl;
	m_dlgNodeBCs = new CNodeBCsDlg();
	m_dlgNodeBCs->setDialogMgr(this);

	cout << "DialogMgr: Creating scale factor dialog." << endl;
	m_dlgScalefactor = new CScalefactorDlg();
	m_dlgScalefactor->setDialogMgr(this);

	cout << "DialogMgr: Creating structure dialog." << endl;
	m_dlgStructure = new CStructureDlg();
}

// ------------------------------------------------------------
CDialogMgr::~CDialogMgr()
{
	cout << "DialogMgr: Destructor." << endl;
	cout << "DialogMgr: Deleting node properties dialog." << endl;
	delete m_dlgNodeProp;
	cout << "DialogMgr: Deleting beam properties dialog." << endl;
	delete m_dlgBeamProp;
	cout << "DialogMgr: Deleting materials dialog." << endl;
	delete m_dlgMaterials;
	cout << "DialogMgr: Deleting element load dialog." << endl;
	delete m_dlgElementLoads;
	cout << "DialogMgr: Deleting node load dialog." << endl;
	delete m_dlgNodeLoads;
	cout << "DialogMgr: Deleting node bc dialog." << endl;
	delete m_dlgNodeBCs;
	cout << "DialogMgr: Deleting scale factor dialog." << endl;
	delete m_dlgScalefactor;
	cout << "DialogMgr: Deleting structure dialog." << endl;
	delete m_dlgStructure;
}

// ------------------------------------------------------------
void CDialogMgr::assignMaterialToSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->assignMaterialToSelected();
}

// ------------------------------------------------------------
void CDialogMgr::removeMaterialFromSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->removeMaterialFromSelected();	
}

// ------------------------------------------------------------
CNodePropDlg* CDialogMgr::getNodePropDlg()
{
	return m_dlgNodeProp;
}

// ------------------------------------------------------------
CBeamPropDlg* CDialogMgr::getBeamPropDlg()
{
	return m_dlgBeamProp;
}

// ------------------------------------------------------------
CMaterialsDlg* CDialogMgr::getMaterialsDlg()
{
	return m_dlgMaterials;
}

// ------------------------------------------------------------
void CDialogMgr::setWidget(void *widget)
{
	m_widget = (CIvfFemWidget*)widget;
	
	m_dlgNodeProp->setWorkspace((CIvfFemWidget*)m_widget);
	m_dlgBeamProp->setWorkspace((CIvfFemWidget*)m_widget);
	m_dlgMaterials->setWorkspace((CIvfFemWidget*)m_widget);
	m_dlgElementLoads->setWorkspace((CIvfFemWidget*)m_widget);
	m_dlgNodeLoads->setWorkspace((CIvfFemWidget*)m_widget);
	m_dlgNodeBCs->setWorkspace((CIvfFemWidget*)m_widget);
}

// ------------------------------------------------------------
void CDialogMgr::setBeamPropDlg(CBeamPropDlg *dlg)
{
	m_dlgBeamProp = dlg;
}

// ------------------------------------------------------------
void CDialogMgr::setNodePropDlg(CNodePropDlg *dlg)
{
	m_dlgNodeProp = dlg;
}

// ------------------------------------------------------------
void CDialogMgr::setMaterialsDlg(CMaterialsDlg *dlg)
{
	m_dlgMaterials = dlg;
}

// ------------------------------------------------------------
void CDialogMgr::setElementLoadsDlg(CElementLoadsDlg *dlg)
{
	m_dlgElementLoads = dlg;
}

// ------------------------------------------------------------
CElementLoadsDlg* CDialogMgr::getElementLoadsDlg()
{
	return m_dlgElementLoads;
}

// ------------------------------------------------------------
void CDialogMgr::hideAll()
{
	m_dlgNodeProp->hide();
	m_dlgBeamProp->hide();
	m_dlgMaterials->hide();
	m_dlgElementLoads->hide();
	m_dlgNodeLoads->hide();
	m_dlgNodeBCs->hide();
}

// ------------------------------------------------------------
void CDialogMgr::addBeamLoad(CFemBeamLoad* elementLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->addBeamLoad(elementLoad);
}

// ------------------------------------------------------------
void CDialogMgr::deleteBeamLoad(CFemBeamLoad* elementLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->deleteBeamLoad(elementLoad);
}

// ------------------------------------------------------------
void CDialogMgr::setCurrentBeamLoad(CFemBeamLoad* elementLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setCurrentBeamLoad(elementLoad);
}

// ------------------------------------------------------------
void CDialogMgr::assignBeamLoadSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->assignBeamLoadSelected();
}

// ------------------------------------------------------------
void CDialogMgr::addNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->addNodeLoad(nodeLoad);
}

// ------------------------------------------------------------
void CDialogMgr::assignNodeLoadSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->assignNodeLoadSelected();
}

// ------------------------------------------------------------
void CDialogMgr::setCurrentNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setCurrentNodeLoad(nodeLoad);
}

// ------------------------------------------------------------
void CDialogMgr::deleteNodeLoad(CFemBeamNodeLoad *nodeLoad)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->deleteNodeLoad(nodeLoad);
}

// ------------------------------------------------------------
CNodeLoadsDlg* CDialogMgr::getNodeLoadsDlg()
{
	return m_dlgNodeLoads;
}

void CDialogMgr::setNodeLoadsDlg(CNodeLoadsDlg *dlg)
{
	m_dlgNodeLoads = dlg;
}

void CDialogMgr::setRotationSelected(double rotation)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setRotationSelected(rotation);
}

void CDialogMgr::setCurrentNodeBC(CFemBeamNodeBC *bc)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setCurrentNodeBC(bc);
}

void CDialogMgr::setNodeBCsDlg(CNodeBCsDlg *dlg)
{
	m_dlgNodeBCs = dlg;
}

CNodeBCsDlg* CDialogMgr::getNodeBCsDlg()
{
	return m_dlgNodeBCs;
}

void CDialogMgr::addNodeBC(CFemBeamNodeBC *bc)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->addNodeBC(bc);
}

void CDialogMgr::deleteNodeBC(CFemBeamNodeBC *bc)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->deleteNodeBC(bc);
}

void CDialogMgr::assignNodeBCSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->assignNodeBCSelected();
}

void CDialogMgr::setScalefactor(double scalefactor)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setScalefactor(scalefactor);
}

double CDialogMgr::getScalefactor()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	return femWidget->getScalefactor();
}

CScalefactorDlg* CDialogMgr::getScalefactorDlg()
{
	return m_dlgScalefactor;
}

CStructureDlg* CDialogMgr::getStructureDlg()
{
	return m_dlgStructure;
}

void CDialogMgr::setNeedRecalc(bool flag)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->setNeedRecalc(flag);
}

void CDialogMgr::removeNodeLoadsFromSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->removeNodeLoadsFromSelected();
}

void CDialogMgr::removeNodeBCsFromSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->removeNodeBCsFromSelected();
}

void CDialogMgr::removeBeamLoadsFromSelected()
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_widget;
	femWidget->removeBeamLoadsFromSelected();
}
