TEMPLATE =  app
CONFIG   =  debug

INCLUDEPATH += $(GLUT_PATH)/include
INCLUDEPATH += $(GLTT_PATH)
INCLUDEPATH += $(TTF_PATH)/lib
INCLUDEPATH += $(IVF_PATH)/include
INCLUDEPATH += $(IVF_PATH)/include/ivf
INCLUDEPATH += $(IVF_PATH)/include/ivfui
INCLUDEPATH += $(IVF_PATH)/include/glm
INCLUDEPATH += $(IVF_PATH)/include/gltx
INCLUDEPATH += $(IVF_PATH)/include/matrix
INCLUDEPATH += $(IVF_PATH)/include/ivffltkwidget
INCLUDEPATH += $(FLTK_PATH)
INCLUDEPATH += ../../include
INCLUDEPATH += ../../include/fem
INCLUDEPATH += ../../include/visfem
INCLUDEPATH += ../../include/util
INCLUDEPATH += ../../../bmsolver/include/generated
INCLUDEPATH += /usr/local/include

DEFINES = HAVE_CONFIG_H

TMAKE_LIBS += -L$(GLUT_PATH)/lib/glut.n32 \
	-L$(GLTT_PATH)/.libs \
	-L$(TTF_PATH)/lib/.libs \
	-L$(GLUT_PATH)/lib/mui.n32 \
	-L$(GLUT_PATH)/lib/gle.n32 \
	-L$(FLTK_PATH)/lib \
	-L$(IVF_PATH)/lib \
	-L/usr/local/lib \
	-L../../lib \
	-lfem -lvisfem -livffltkwidget -livf -lgltx -lfltk -lglut \
	-lOB -lCosNaming \
	-lgle -lgltt -lttf -lGL -lGLU  -lXext -lX11 -lc -lm

HEADERS  = BeamPropDlg.h \
	IvfPlaneButton.h \
	IvfButtonGroup.h \
	IvfArea2D.h \
	IvfFemWidget.h \
	NodeLoadDlg.h \
	DialogMgr.h \
	MainFrame.h \
	NodeLoadsDlg.h \
	ElementLoadDlg.h \
	MaterialDlg.h \
	NodePropDlg.h \
	ElementLoadsDlg.h \
	MaterialsDlg.h \
	NodeBCsDlg.h \
	NodeBCDlg.h \
	FemDFEMCInterface.h \
	ScalefactorDlg.h \
	dfemc.h \
	WorkspaceDlg.h 

SOURCES  = BeamPropDlg.cpp \
	IvfPlaneButton.cpp \
	IvfButtonGroup.cpp \
	IvfArea2D.cpp \
	IvfFemWidget.cpp \
	NodeLoadDlg.cpp \
	DialogMgr.cpp \
	MainFrame.cpp \
	NodeLoadsDlg.cpp \
	ElementLoadDlg.cpp \
	MaterialDlg.cpp \
	NodePropDlg.cpp \
	ElementLoadsDlg.cpp \
	MaterialsDlg.cpp \
	WorkspaceDlg.cpp \
	NodeBCsDlg.cpp \
	NodeBCDlg.cpp \
	FemDFEMCInterface.cpp \
	ScalefactorDlg.cpp \
	../../../bmsolver/include/generated/dfemc.cpp \
	main.cpp

TARGET   =  objframe

