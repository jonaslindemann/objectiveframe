//
//  LeapListener.h
//  objframe2
//
//  Created by Daniel Åkesson on 8/28/13.
//
//

#ifndef __objframe2__LeapListener__
#define __objframe2__LeapListener__

#include <iostream>
#include "IvfFemWidget.h"

using namespace Leap;

class LeapListener : public Listener {
private:
    CIvfFemWidget *m_workspace;
    int lastID;
    int* gesturePointer;
    int gesture;
public:
    void setWorkspace(CIvfFemWidget *workspace);
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
};

#endif /* defined(__objframe2__LeapListener__) */
