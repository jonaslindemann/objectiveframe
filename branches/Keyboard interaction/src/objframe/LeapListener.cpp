//
//  LeapListener.cpp
//  objframe2
//
//  Created by Daniel Åkesson on 8/28/13.
//
//

#include "LeapListener.h"

void LeapListener::setWorkspace(CIvfFemWidget *workspace) {
    m_workspace = workspace;
}

void LeapListener::onInit(const Controller& controller) {
    std::cout << "Initialized" << std::endl;
    gesturePointer = new int;
    gesturePointer = &gesture;
    gesture = GEST_NONE;
    lastID = 0;
    
}

void LeapListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
//    controller.enableGesture(Gesture::TYPE_CIRCLE);
//    controller.enableGesture(Gesture::TYPE_KEY_TAP);
//    controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapListener::onDisconnect(const Controller& controller) {
    //Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller& controller) {
    std::cout << "Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {

    //Grasp & Spread gestures
    
    bool oneFingerNow = true;
    bool twoFingerBefore = true;
    
    int numberOfFrames = 10;
    int inBetweenFrames = 10;
    
    double palmVelocity = controller.frame().hands()[0].palmVelocity().magnitude();
        
    for (int i=0;i<numberOfFrames;i++)
    {
        if (controller.frame(i).fingers().count() != 1)
            oneFingerNow = false;
    }
    
    for (int i=numberOfFrames+inBetweenFrames;i<numberOfFrames*2+inBetweenFrames;i++)
    {
        if (controller.frame(i).fingers().count() != 2)
            twoFingerBefore = false;
    }
    
    
    if (oneFingerNow && twoFingerBefore && palmVelocity<40 && lastID + 20 < controller.frame().id())
    {
        gesture = GEST_PINCH_HAND1;
        lastID = controller.frame().id();
        cout << "Pinch hand 1" << endl;
    }
    
    
    bool oneFingerBefore = true;
    bool moreFingerNow = true;
    
    for (int i=0;i<numberOfFrames;i++)
    {
        if (controller.frame(i).fingers().count() < 2)
            moreFingerNow = false;
    }
    
    for (int i=numberOfFrames+inBetweenFrames;i<numberOfFrames*2+inBetweenFrames;i++)
    {
        if (controller.frame(i).fingers().count() != 1)
            oneFingerBefore = false;
    }
    
    
    if (oneFingerBefore && moreFingerNow && palmVelocity<40 && lastID + 20 < controller.frame().id())
    {
        gesture = GEST_SPREAD_HAND1;
        lastID = controller.frame().id();        
        cout << "Spred hand 1" << endl;
    }

    
    
    // Get gestures
    const GestureList leapGestures = controller.frame().gestures();
    for (int g = 0; g < leapGestures.count(); ++g) {
        Gesture leapGesture = leapGestures[g];
        
        if (leapGesture.type() == Gesture::TYPE_SWIPE)
            {
                SwipeGesture swipe = leapGesture;

                if (swipe.direction().y > 0.8 && leapGesture.state() == 1 && lastID+40 < controller.frame().id())
                {
                    gesture = GEST_SWIPE_UP;
                    lastID = controller.frame().id();
                    cout << "Swipe up"<<endl;

                } else if (swipe.direction().y < -0.8 && leapGesture.state() == 1 && lastID+40 < controller.frame().id())
                {
                    gesture = GEST_SWIPE_DOWN;
                    lastID = controller.frame().id();                    
                    cout << "Swipe down"<<endl;
                } else if (swipe.direction().x < -0.8 && leapGesture.state() == 1 && lastID+40 < controller.frame().id())
                {
                    gesture = GEST_SWIPE_LEFT;
                    lastID = controller.frame().id();
                    cout << "Swipe left"<<endl;
                }  else if (swipe.direction().x >  0.8 && leapGesture.state() == 1 && lastID+40 < controller.frame().id())
                {
                    gesture = GEST_SWIPE_RIGHT;
                    lastID = controller.frame().id();
                    cout << "Swipe right"<<endl;
                }
//                
//                std::cout << "Swipe id: " << swipe.id()
//                << ", state: " << swipe.state()
//                << ", direction: " << swipe.direction()
//                << ", speed: " << swipe.speed() << std::endl;
                
                
            }
      }
    
    
    m_workspace->updateLeapController(controller, gesturePointer);
    
}



void LeapListener::onFocusGained(const Controller& controller) {
    std::cout << "Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller& controller) {
    std::cout << "Focus Lost" << std::endl;
}
