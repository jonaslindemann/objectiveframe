// generated by Fast Light User Interface Designer (fluid) version 1.0300

#include "BeamPropDlg.h"
#include "MaterialDlg.h"
#include "IvfFemWidget.h"

void CBeamPropDlg::cb_btnClose_i(Fl_Button*, void*) {
    wndBeamProp->hide();
}
void CBeamPropDlg::cb_btnClose(Fl_Button* o, void* v) {
    ((CBeamPropDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

void CBeamPropDlg::cb_sldBeamRotation_i(Fl_Value_Slider*, void*) {
    CIvfFemWidget* femWidget = (CIvfFemWidget*)m_femWidget;
    femWidget->setRotationSelected(sldBeamRotation->value());
}
void CBeamPropDlg::cb_sldBeamRotation(Fl_Value_Slider* o, void* v) {
    ((CBeamPropDlg*)(o->parent()->parent()->user_data()))->cb_sldBeamRotation_i(o,v);
}

void CBeamPropDlg::cb_btnShowMaterialDlg_i(Fl_Button*, void*) {
    if (this->getBeam()->getMaterial()!=NULL)
    {
        CMaterialDlg* dlg = new CMaterialDlg();
        dlg->setMaterial(this->getBeam()->getBeam()->getMaterial());
        dlg->setWorkspace((Fl_Widget*)m_femWidget);
        dlg->show();
        if (dlg->getModalResult()==MR_OK)
        {
            //ivfWorkspace->setWorkspace(dlg->getWorkspaceSize());
        }
        delete dlg;
        CIvfFemWidget* femWidget = (CIvfFemWidget*)m_femWidget;
        femWidget->setNeedRecalc(true);
    };
}
void CBeamPropDlg::cb_btnShowMaterialDlg(Fl_Button* o, void* v) {
    ((CBeamPropDlg*)(o->parent()->parent()->user_data()))->cb_btnShowMaterialDlg_i(o,v);
}

CBeamPropDlg::CBeamPropDlg() {
    {   wndBeamProp = new Fl_Double_Window(212, 360, "Beam information");
        wndBeamProp->user_data((void*)(this));
        {   btnClose = new Fl_Button(72, 330, 61, 25, "Close");
            btnClose->down_box(FL_DOWN_BOX);
            btnClose->labelsize(12);
            btnClose->callback((Fl_Callback*)cb_btnClose);
        } // Fl_Button* btnClose
        {   grpBeamRotation = new Fl_Group(4, 4, 204, 93);
            grpBeamRotation->box(FL_DOWN_FRAME);
            {   sldBeamRotation = new Fl_Value_Slider(12, 24, 188, 24, "Beam rotation");
                sldBeamRotation->type(1);
                sldBeamRotation->labelsize(12);
                sldBeamRotation->maximum(360);
                sldBeamRotation->step(5);
                sldBeamRotation->callback((Fl_Callback*)cb_sldBeamRotation);
                sldBeamRotation->align(Fl_Align(FL_ALIGN_TOP_LEFT));
            } // Fl_Value_Slider* sldBeamRotation
            {   btnShowMaterialDlg = new Fl_Button(12, 61, 188, 27, "Edit assigned beam property");
                btnShowMaterialDlg->down_box(FL_DOWN_BOX);
                btnShowMaterialDlg->labelsize(12);
                btnShowMaterialDlg->callback((Fl_Callback*)cb_btnShowMaterialDlg);
            } // Fl_Button* btnShowMaterialDlg
            {   boxDivider = new Fl_Box(12, 54, 188, 2);
                boxDivider->box(FL_ENGRAVED_BOX);
            } // Fl_Box* boxDivider
            grpBeamRotation->end();
        } // Fl_Group* grpBeamRotation
        {   tabResults = new Fl_Tabs(4, 104, 204, 221);
            tabResults->box(FL_UP_BOX);
            {   grpSectionForces = new Fl_Group(8, 127, 199, 196, "Section forces");
                grpSectionForces->labelsize(12);
                grpSectionForces->hide();
                {   lbSectionForces = new Fl_Browser(9, 131, 194, 189);
                    lbSectionForces->labelsize(12);
                    lbSectionForces->textfont(4);
                    lbSectionForces->textsize(10);
                } // Fl_Browser* lbSectionForces
                grpSectionForces->end();
            } // Fl_Group* grpSectionForces
            {   grpDisplacements = new Fl_Group(6, 128, 200, 194, "Displacements");
                grpDisplacements->labelsize(12);
                {   lbDisplacements = new Fl_Browser(9, 131, 194, 189);
                    lbDisplacements->textfont(4);
                    lbDisplacements->textsize(10);
                } // Fl_Browser* lbDisplacements
                grpDisplacements->end();
            } // Fl_Group* grpDisplacements
            tabResults->end();
        } // Fl_Tabs* tabResults
        wndBeamProp->set_non_modal();
        wndBeamProp->end();
    } // Fl_Double_Window* wndBeamProp
    m_beam = NULL;
    m_femWidget = NULL;
}

void CBeamPropDlg::show() {
    this->setBeam(m_beam);
    wndBeamProp->show();
}

void CBeamPropDlg::setBeam(CIvfFemBeam* beam) {
    m_beam = beam;
    if (m_beam==NULL)
    {
        btnShowMaterialDlg->deactivate();
        sldBeamRotation->deactivate();
        lbSectionForces->clear();
        lbDisplacements->clear();
    }
    else
    {
        btnShowMaterialDlg->activate();
        sldBeamRotation->activate();
        sldBeamRotation->value(m_beam->getBeam()->getBeamRotation());
        this->fillListboxes();
    }
}

CIvfFemBeam* CBeamPropDlg::getBeam() {
    return m_beam;
}

void CBeamPropDlg::hide() {
    wndBeamProp->hide();
}

void CBeamPropDlg::setFemWidget(void* femWidget) {
    m_femWidget = femWidget;
}

void CBeamPropDlg::fillListboxes() {
    if (m_beam!=NULL)
    {
        if (m_beam->getBeam()->getValueSize()>0)
        {
            int k,j;
            int pos = 0;
            double values[6];
            char buffer[200];
            int n = m_beam->getBeam()->getEvaluationPoints();
            lbSectionForces->clear();
            lbSectionForces->add("      N          Vy         Vz         T          My         Mz");
            lbDisplacements->clear();
            lbDisplacements->add("      u          v          w          fi");
            for (k=1; k<=n; k++)
            {
                for (j=1; j<=6; j++)
                {
                    values[j-1] = m_beam->getBeam()->getValue(pos++);
                }
                sprintf(
                    buffer,
                    "%10.3f %10.3f %10.3f %10.3f %10.3f %10.3f",
                    values[0],values[1],values[2],values[3],values[4],values[5]);
                lbSectionForces->add(buffer);
            }

            for (k=1; k<=n; k++)
            {
                for (j=1; j<=4; j++)
                {
                    values[j-1] = m_beam->getBeam()->getValue(pos++);
                }
                sprintf(
                    buffer,
                    "%10.3f %10.3f %10.3f %10.3f",
                    values[0],values[1],values[2],values[3]);
                lbDisplacements->add(buffer);
            }
        }
    }
}
