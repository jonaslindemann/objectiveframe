//
//  LeapInteraction.cpp
//  objframe2
//
//  Created by Daniel Åkesson on 9/2/13.
//
//

#include "LeapInteraction.h"

LeapInteraction::LeapInteraction()
{
    m_lastDistance = 10000;
}

LeapInteraction::LeapInteraction(CIvfFemWidget *widget)
{
    m_widget=widget;
    
    m_finger1 = new LeapFinger(widget);
    m_finger2 = new LeapFinger(widget);
    m_forceVector = new CIvfVec3d;

}

LeapInteraction::~LeapInteraction()
{
    
}

void LeapInteraction::updateLeapFrame(Frame leapFrame)
{
    m_leapFrame = leapFrame;
}

void LeapInteraction::refresh()
{
    m_widget->getCamera()->getAbsoluteRotation(m_alpha, m_beta);
    Finger finger = m_leapFrame.fingers()[0];
    if (m_leapFrame.fingers().count() == 2 && m_widget->getTactileForce()->getState()==CIvfShape::OS_ON){

    } else if (m_leapFrame.fingers().count() == 2 && m_widget->getTactileForce()->getState()==CIvfShape::OS_OFF) {
        //this->fingerMove();
    }
    
    this->viewInteraction();
    
    if (m_leapFrame.fingers().count() == 2)
    {
        m_finger1->fingerMove(m_leapFrame.fingers()[0]);
        m_finger2->fingerMove(m_leapFrame.fingers()[1]);
        m_finger1->Show(true);
        m_finger2->Show(true);
    } else {
        m_finger1->Show(false);
        m_finger2->Show(false);
    }
    
    graspGesture();
    
    //Redraw
    if (m_leapFrame.fingers().count() > 0)
        m_widget->redraw();
    

}

void LeapInteraction::viewInteraction()
{
    if (m_leapFrame.hands().count() == 1)
    {
        Hand hand = m_leapFrame.hands()[0];
        if (hand.fingers().count() == 5)
        {
            m_widget->getCamera()->rotateAbsolute(hand.direction().pitch()*1.2, m_beta);
            
            m_widget->getCamera()->moveForward(hand.palmVelocity()[2]/250);
            m_widget->getCamera()->rotatePositionY(hand.palmVelocity()[0]/4500);

        }
        
        
    } else if (m_leapFrame.hands().count() == 2) {
        if (m_leapFrame.hands()[0].fingers().count() == 5 && m_leapFrame.hands()[1].fingers().count() == 5)
            //Two hands showing with all fingers
        {
            Vector palmVelocity = (m_leapFrame.hands()[0].palmVelocity() + m_leapFrame.hands()[1].palmVelocity())/2;
            m_widget->getCamera()->moveSideways(palmVelocity[0]/250);
            m_widget->getCamera()->moveDepth(palmVelocity[2]/250);
            m_widget->getCamera()->moveVertical(-palmVelocity[1]/2500);
        }
    }

}



void LeapInteraction::findNode(double v[3], double &distance, CIvfFemNode* &closestNode)
{
    distance = 100000;

    double coord[3];
    double tempDistance;
    
    for (int i=0; i<m_widget->getScene()->getComposite()->getSize() ; i++)
    {
        CIvfShape* shape = m_widget->getScene()->getComposite()->getChild(i);
        if (shape->isClass("CIvfFemNode"))
        {
            CIvfFemNode* ivfNode = (CIvfFemNode*)shape;
            
            ivfNode->getFemNode()->getCoord(coord[0],coord[1],coord[2]);
            
            tempDistance = sqrt(pow((v[0]-coord[0]),2)+pow((v[1]-coord[1]),2)+pow((v[2]-coord[2]),2));
            
            if (tempDistance<distance)
            {
                distance = tempDistance;
                closestNode = ivfNode;
            }

        }
    }

}

void LeapInteraction::graspGesture()
{
//    if (m_leapFrame.fingers().count() == 2)
//    {
//        Vector p1 = m_leapFrame.fingers()[0].stabilizedTipPosition();
//        Vector p2 = m_leapFrame.fingers()[1].stabilizedTipPosition();
//        
//        Vector dir1 = m_leapFrame.fingers()[0].direction();
//        Vector dir2 = m_leapFrame.fingers()[1].direction();
//        
//        dir1.normalized()*20;
//        dir2.normalized()*20;
//        
//        dir1.y=0;
//        dir2.y=0;
//        
//        Vector ip;
//        
//        double distance = this->vectorsIntersect(p1, p2, dir1, dir2);
    bool grasp,spread;
    double palmVelocity;
    palmVelocity = m_leapFrame.hands()[0].palmVelocity().magnitude();

    if (m_leapFrame.fingers().count() == 1 && m_lastFingerCount == 2 && palmVelocity < 160)
    {
        grasp = true;
    } else {
        grasp = false;
    }
    
    if (m_leapFrame.fingers().count() > 1)
    {
        spread = true;
    } else {
        spread = false;
    }
    
    m_lastFingerCount = m_leapFrame.fingers().count();
    
    if (grasp)
    {
        startGrasp();
    }
    
    if (spread)
    {
        endGrasp();
        m_interact = false;
    }
    
    if (m_interact)
        interactNode();
    

}

double LeapInteraction::vectorsIntersect(Vector p1, Vector p2, Vector dir1, Vector dir2)
{
    
    Vector   u = dir1;
    Vector   v = dir2;
    Vector   w = p1-p2;
    float    a = u.dot(u);         // always >= 0
    float    b = u.dot(v);
    float    c = v.dot(v);         // always >= 0
    float    d = u.dot(w);
    float    e = v.dot(w);
    float    D = a*c - b*b;        // always >= 0
    float    sc, tc;
    
    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) {          // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);    // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }
    
    // get the difference of the two closest points
    Vector   dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)
    
    return dP.magnitude();   // return the closest distance
    
}

void LeapInteraction::startGrasp()
{

    double searchP[3];
    m_finger1->getPosition().getComponents(searchP[0],searchP[1],searchP[2]);

    double distance = 10000;
    CIvfFemNode *closestNode;
    
    findNode(searchP, distance, closestNode);

    if (distance < SNAP_DIST)
    {
        m_interactionNode = closestNode;
        m_widget->setInteractionNode(closestNode);
        
        m_interactionNode->setSelect(CIvfShape::SS_ON);
        m_widget->getTactileForce()->setState(CIvfShape::OS_ON);

        double x,y,z;
        m_interactionNode->getFemNode()->getCoord(x, y, z);
        m_finger1->setPosition(x, y, z);
        m_finger2->setPosition(x, y, z);
        
        m_graspPoint = m_leapFrame.hands()[0].palmPosition();
        
        m_interact = true;
    } else {
        m_interact = false;
    }
}

void LeapInteraction::interactNode()
{
    if (m_interact)
    {
        
        Vector currentP = m_leapFrame.hands()[0].palmPosition();
        Vector zeroP;
        
        zeroP.y = 250;
        Vector forceV = currentP - zeroP;
        
        LeapToScene(forceV, m_forceVector);
        
        double magnitude = forceV.magnitude()/200;
        double fx,fy,fz;
        
        m_forceVector->getComponents(fx, fy, fz);
        
        m_widget->getTactileForce()->setDirection(fx, fy, fz);
//        m_widget->getTactileForce()->setSize(magnitude, magnitude*0.2);
//        m_widget->getTactileForce()->setOffset(-magnitude);
        
        m_widget->doFeedback();
        m_widget->redraw();

        
    }
}

void LeapInteraction::endGrasp()
{
    if (m_interactionNode)
    {
        m_interactionNode->setSelect(CIvfShape::SS_OFF);

        m_widget->doFeedback();
        m_widget->redraw();
    }
}



void LeapInteraction::LeapToScene(Vector leapVector, CIvfVec3d* returnVector)
{
    
    returnVector->setX(leapVector.x);
    returnVector->setY(leapVector.y);
    returnVector->setZ(leapVector.z);
    
    CIvfVec3d axis;
    axis.add(0, 1, 0);
    
    returnVector->rotate(axis, 90-m_beta*RAD_TO_DEG);
}

Vector LeapInteraction::adjustPosition(Vector inputVector)
{
    Vector returnVector;
    returnVector = inputVector*leapSpeed;
    
    //Keep in grid but not under it
    returnVector.y = inputVector.y*leapHeightSpeed;
    
    returnVector.y=returnVector.y-2;
    
    if (returnVector.y < 0)
        returnVector.y=0;
    
    return returnVector;
    
}
