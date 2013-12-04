//
//  LeapInteraction.h
//  objframe2
//
//  Created by Daniel Åkesson on 9/2/13.
//
//

#ifndef _CLeapInteraction_H_
#define _CLeapInteraction_H_

#include <iostream>

#include "IvfFemWidget.h"
#include "LeapFinger.h"

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
#define SNAP_DIST 1
#define ANIMATE_SPEED 10

#ifdef USE_LEAP

using namespace Leap;

class LeapInteraction {
private:
    int                 m_editMode;
    double              m_alpha, m_beta;
    
    Frame               m_leapFrame;
    Controller          m_leapController;
    CIvfFemWidget       *m_widget;
    
    LeapFinger          *m_finger1;
    LeapFinger          *m_finger2;
    
    CIvfVec3d           *m_forceVector;
    
    bool                m_interact;
    int                 *m_gesture;
    
    CIvfFemNode         *m_interactionNode;
    CIvfNode            *m_startDraw;
    CIvfNode            *m_endDraw;
    CIvfShape*          newLine;
    CIvfFemNode         *m_moveNode;
    
    //Menu
    vector<int>         m_editModes;
    vector<CIvfPlaneButton*> m_buttons;
    CIvfButtonGroup* m_objectButtons;
    bool                menuUp;
    
    double              m_animation;
    
public:
    LeapInteraction();
    LeapInteraction(CIvfFemWidget *widget);
    virtual ~LeapInteraction();
    
    void updateLeapController(const Controller& leapController, int* gesture);
    void refresh();
    
    //Menu
    void setupOverlay();
    void reCheck();
    void animateMenuUp();
    void animateMenuDown();
    void interactMenu();
    
    void viewInteraction();
    void highlightCloseNodes();
    void findNode(double v[3], double &distance, CIvfFemNode* &closestNode);
    void gestures();
    bool grasp();
    
    
    void startGrasp();
    void interactNode();
    void endGrasp();
    
    void LeapToScene(Vector leapVector, CIvfVec3d* returnVector);
    Vector adjustPosition(Vector inputVector);
};

#endif
#endif