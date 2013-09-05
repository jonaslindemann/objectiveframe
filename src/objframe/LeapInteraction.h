//
//  LeapInteraction.h
//  objframe2
//
//  Created by Daniel Åkesson on 9/2/13.
//
//

#ifndef _CLeapInteraction_H_
#define _CLeapInteraction_H_

#include "ObjframeConfig.h"

#ifdef USE_LEAP

#include <iostream>
#include "Leap.h"
#include "IvfFemWidget.h"
#include "LeapFinger.h"

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
#define SNAP_DIST 8

using namespace Leap;

class LeapInteraction {
private:
    double              m_alpha, m_beta;
    
    Frame               m_leapFrame;
    CIvfFemWidget       *m_widget;
    
    LeapFinger          *m_finger1;
    LeapFinger          *m_finger2;
    
    CIvfVec3d           *m_forceVector;
    
    double              m_lastFingerCount;
    double              m_lastDistance;
    bool                m_interact;
    
    Vector              m_graspPoint;
    CIvfFemNode         *m_interactionNode;
    
    
public:
    LeapInteraction();
    LeapInteraction(CIvfFemWidget *widget);
    virtual ~LeapInteraction();
    
    void updateLeapFrame(Frame leapFrame);
    void refresh();
    void viewInteraction();
    void findNode(double v[3], double &distance, CIvfFemNode* &closestNode);
    void graspGesture();
    double vectorsIntersect(Vector p1, Vector p2, Vector dir1, Vector dir2);
    
    void startGrasp();
    void interactNode();
    void endGrasp();

    void LeapToScene(Vector leapVector, CIvfVec3d* returnVector);
    Vector adjustPosition(Vector inputVector);
};

#endif

#endif
