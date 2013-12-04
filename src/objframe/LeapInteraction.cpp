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
    m_animation = 0;
}

LeapInteraction::LeapInteraction(CIvfFemWidget *widget)
{
    m_widget=widget;
    
    m_finger1 = new LeapFinger(widget);
    m_finger2 = new LeapFinger(widget);

    //m_finger1->highlight();
    m_forceVector = new CIvfVec3d;
    
    setupOverlay();
    m_editMode = LEAP_MOVE;
    reCheck();

}

LeapInteraction::~LeapInteraction()
{
    
}

void LeapInteraction::updateLeapController(const Controller& leapController, int* gesture)
{
    m_leapFrame = leapController.frame();
    m_gesture = gesture;
}

void LeapInteraction::refresh()
{
    if (m_gesture)
    {
        m_widget->getCamera()->getAbsoluteRotation(m_alpha, m_beta);
        bool menu = menuUp || *m_gesture == GEST_SWIPE_UP || *m_gesture == GEST_SWIPE_DOWN; 
        
        if (!menu)
        {
            this->viewInteraction();
        } else {
            this->interactMenu();
        }
        
        if (m_leapFrame.fingers().count() == 2 && !m_interact && !menu)
        {
            m_finger1->fingerMove(m_leapFrame.fingers()[0]);
            //m_finger2->fingerMove(m_leapFrame.fingers()[1]);
            m_finger1->show(true);
            //m_finger2->show(true);
            
            highlightCloseNodes();
            
        } else {
            m_finger1->show(false);
            m_finger2->show(false);
        }
        
        gestures();
        
        //Redraw
        if (m_leapFrame.fingers().count() > 0)
            m_widget->redraw();
    
    }
}


void LeapInteraction::setupOverlay()
{
    m_editModes.push_back(LEAP_LINE);
    m_editModes.push_back(LEAP_MOVE);
    m_editModes.push_back(LEAP_INTERACT);
    
    m_widget->removeMenus();
    
    CIvfPlaneButton* button;

    
    m_objectButtons = new CIvfButtonGroup();
    
    button= new CIvfPlaneButton(LEAP_LINE, "images/tlsolidline.png");
    button->setSize(50.0,50.0);
    button->setPosition(30.0,30.0,0.0);
    button->setHint("Create element");
    m_objectButtons->addChild(button);

    button= new CIvfPlaneButton(LEAP_MOVE, "images/tlmove.png");
    button->setSize(50.0,50.0);
    button->setPosition(90.0,30.0,0.0);
    button->setHint("Move nodes or elements");
    m_objectButtons->addChild(button);
    
    button= new CIvfPlaneButton(LEAP_INTERACT, "images/tlnodeloads.png");
    button->setSize(50.0,50.0);
    button->setPosition(150.0,30.0,0.0);
    button->setHint("Show node loads");
    m_objectButtons->addChild(button);
    
    
    m_objectButtons->setPosition(m_widget->w()/2-200,m_widget->h()-70,0.0);
    m_widget->getOverlay()->addChild(m_objectButtons);
    
}

void LeapInteraction::reCheck()
{
    for (int i=0;i<m_editModes.size();i++)
    {
        if (m_editMode == m_editModes[i])
        {
            CIvfGenericButton* button = (CIvfGenericButton*) m_objectButtons->getChild(i);
            button->setButtonState(CIvfGenericButton::BS_CHECKED);
            m_editMode = m_editModes[i];
        } else {
            CIvfGenericButton* button = (CIvfGenericButton*) m_objectButtons->getChild(i);
            button->setButtonState(CIvfGenericButton::BS_NORMAL);
        }
    }

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

void LeapInteraction::highlightCloseNodes()
{
    double v[3],v1[3],v2[3];
    
    m_finger1->getPosition().getComponents(v1[0], v1[1], v1[2]);
    m_finger2->getPosition().getComponents(v2[0], v2[1], v2[2]);
    
    for (int i=0;i<3;i++)
    {
        v[i]=(v1[i]+v2[i])/2;
    }
    
    
    if (m_interactionNode != NULL)
        m_interactionNode->setHighlight(CIvfShape::HS_OFF);
    
    double distance;
    findNode(v1, distance, m_interactionNode);
    
    if (distance < SNAP_DIST)
    {
        m_interactionNode->setHighlight(CIvfShape::HS_ON);
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
            
            //Turn off highlight on all
            ivfNode->setHighlight(CIvfShape::HS_OFF);
            
            tempDistance = sqrt(pow((v[0]-coord[0]),2)+pow((v[1]-coord[1]),2)+pow((v[2]-coord[2]),2));
            
            if (tempDistance<distance)
            {
                distance = tempDistance;
                closestNode = ivfNode;
            }

        }
    }

}

void LeapInteraction::gestures()
{

    switch (*m_gesture) {
        case GEST_PINCH_HAND1:
        {
            startGrasp();
            *m_gesture = GEST_NONE;
            break;
        }
        case GEST_SPREAD_HAND1:
        {
            endGrasp();
            m_interact = false;
            *m_gesture = GEST_NONE;
            break;
        }
        case GEST_SWIPE_UP:
        {
            if (!menuUp)
                animateMenuUp();

            break;
        }
        case GEST_SWIPE_DOWN:
        {
            if (menuUp)
                animateMenuDown();
            break;
        }
    }


    
    if (m_interact)
        interactNode();
    

}

void LeapInteraction::animateMenuUp()
{
    double scale = 1+m_animation/ANIMATE_SPEED;
    double menuWidth = m_objectButtons->getSize()*150*scale/2;
        
    double menuHeight = m_widget->h()-70-(m_widget->h()/2+100)/(ANIMATE_SPEED/m_animation);
    m_objectButtons->setPosition(m_widget->w()/2-menuWidth/2,menuHeight,0.0);
    m_animation++;
    

    m_objectButtons->setScale(scale, scale, scale);
    
    if (m_animation == ANIMATE_SPEED)
    {
        menuUp = true;
        *m_gesture = GEST_NONE;
        m_animation = 0;
    }
    
}

void LeapInteraction::animateMenuDown()
{
    double scale = 2-m_animation/ANIMATE_SPEED;
    double menuWidth = m_objectButtons->getSize()*150*scale/2;
    
    double menuHeight = m_widget->h()-70-(m_widget->h()/2+100)/(ANIMATE_SPEED/(ANIMATE_SPEED-m_animation));
    m_objectButtons->setPosition(m_widget->w()/2-menuWidth/2,menuHeight,0.0);
    m_animation++;
    
    m_objectButtons->setScale(scale, scale, scale);
    
    if (m_animation > ANIMATE_SPEED)
    {
        menuUp = false;
        *m_gesture = GEST_NONE;
        m_animation = 0;
    }
}

void LeapInteraction::interactMenu()
{
    double palmPos = m_leapFrame.hands()[0].palmPosition().x;
    palmPos = palmPos+200;
    
    for (int i=0; i<m_editModes.size(); i++)
    {
        double fieldSize = 400/m_editModes.size();
        
        if (palmPos < fieldSize*(i+1) && palmPos > fieldSize*i)
        {
            m_editMode = m_editModes[i];
            reCheck();
        }
    }
    
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
        m_interact = true;
        
        switch (m_editMode) {
            case LEAP_INTERACT:
            {
                m_widget->setInteractionNode(m_interactionNode);
                m_interactionNode->setSelect(CIvfShape::SS_ON);
                m_widget->getTactileForce()->setState(CIvfShape::OS_ON);
                break;
            }
            case LEAP_MOVE:
            {
                m_interactionNode->setSelect(CIvfShape::SS_ON);
                break;
            }
            case LEAP_LINE:
            {
                m_startDraw = closestNode;
            }
        }

    } else {
        
        m_interact = false;

        //Did not snap to a node
        switch (m_editMode) {
            case LEAP_LINE:
            {

                m_widget->onCreateNode(searchP[0],searchP[1],searchP[2], m_startDraw);
                m_widget->getScene()->addChild(m_startDraw);

                break;
            }
        }
    }
    
    //Always run (node found, node not found)
    
    switch (m_editMode) {
        case LEAP_LINE:
        {
            
            double fx,fy,fz;
            m_finger1->getPosition().getComponents(fx, fy, fz);
            
            //Create end node
            m_widget->onCreateNode(fx, fy, fz, m_endDraw);
            m_widget->getScene()->addChild(m_endDraw);
            
            //Create beam element
            newLine = new CIvfShape;
            
            m_widget->onCreateLine(m_startDraw, m_endDraw, newLine);
            m_widget->getScene()->addChild(newLine);
            
            m_interact = true;
            break;
        }
    }
    
}

void LeapInteraction::interactNode()
{
    if (m_interact)
    {
        cout << "Interacting" << m_leapFrame.id() << endl;
        
        switch (m_editMode)
        {
            case LEAP_INTERACT:
            {
                
                Vector currentP = m_leapFrame.hands()[0].palmPosition();
                Vector zeroP;
                
                zeroP.y = 250;
                Vector forceV = currentP - zeroP;
                
                LeapToScene(forceV, m_forceVector);
                
                double fx,fy,fz;
                m_forceVector->getComponents(fx, fy, fz);
                
                m_widget->getTactileForce()->setDirection(fx, fy, fz);
                
                m_widget->setNeedRecalc(true);
                m_widget->set_changed();
                
                m_widget->doFeedback();
                m_widget->redraw();
                break;
            }
            case LEAP_MOVE:
            {
                Vector currentP = m_leapFrame.hands()[0].palmPosition();
                LeapToScene(adjustPosition(currentP), m_forceVector);
                
                double fx,fy,fz;
                m_forceVector->getComponents(fx, fy, fz);

                m_interactionNode->setPosition(fx,fy,fz);

                m_widget->setNeedRecalc(true);
                m_widget->set_changed();
                
                m_widget->doFeedback();
                m_widget->redraw();
                
                break;
            }
            case LEAP_LINE:
            {
                
                
                CIvfVec3d* fingerPos = new CIvfVec3d;
                
                LeapToScene(adjustPosition(m_leapFrame.fingers()[0].stabilizedTipPosition()), fingerPos);

                double pos[3];
                fingerPos->getComponents(pos[0],pos[1],pos[2]);
                
                double distance=10000;
                CIvfFemNode* closestNode;
                findNode(pos, distance, closestNode);
                

                cout << "Distance" << distance << endl;
                m_endDraw->setPosition(pos[0],pos[1],pos[2]);

                m_widget->setNeedRecalc(true);
                m_widget->set_changed();
                
                m_widget->doFeedback();
                
                break;
            }
        }
    }
}

void LeapInteraction::endGrasp()
{
    if (m_interactionNode)
    {
        m_interactionNode->setSelect(CIvfShape::SS_OFF);
        m_interactionNode = NULL;
        m_startDraw = NULL;
        m_endDraw = NULL;
        
        m_widget->set_changed();
        m_widget->setNeedRecalc(true);
        m_widget->doFeedback();
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
