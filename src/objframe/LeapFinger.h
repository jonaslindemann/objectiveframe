//
//  LeapFinger.h
//  objframe2
//
//  Created by Daniel Åkesson on 9/4/13.
//
//

#ifndef __objframe2__LeapFinger__
#define __objframe2__LeapFinger__

#include <iostream>
#include "Leap.h"
#include "IvfFemWidget.h"

#ifdef USE_LEAP

#define leapSpeed 1/20
#define leapHeightSpeed 1/60

using namespace Leap;

class LeapFinger {
private:
    CIvfFemWidget       *m_widget;
    
    double              m_alpha, m_beta;
    Finger              m_fingerData;
    CIvfCylinderPtr     m_fingerShape;
    CIvfVec3d           *m_fingerPos;
    CIvfVec3d           *m_fingerDir;
    CIvfMaterialPtr     material;
    
public:
    LeapFinger(CIvfFemWidget *widget);
    virtual ~LeapFinger();
    void fingerMove(Finger finger);
    void leapToScene(Vector leapVector, CIvfVec3d* sceneVector);
    Vector adjustPosition(Vector height);
    void show(bool show);
    void highlight();
    
    CIvfVec3d getPosition();
    void setPosition(double x, double y, double z);
};

#endif /* defined(__objframe2__LeapFinger__) */

#endif