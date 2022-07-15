#include "Area2D.h"

Area2D::Area2D()
{
    m_visible = true;
}

Area2D::~Area2D()
{
    this->clear();
}

void Area2D::add(int x, int y)
{
    m_xCoords.push_back(x);
    m_yCoords.push_back(y);
    m_red.push_back(0.3f);
    m_green.push_back(0.2f);
    m_blue.push_back(0.0f);
}


void Area2D::clear()
{
    m_xCoords.clear();
    m_yCoords.clear();
    m_red.clear();
    m_green.clear();
    m_blue.clear();
}

void Area2D::getCoord(int idx, int &x, int &y)
{
    if ((idx>=0)&&(idx<m_xCoords.size()))
    {
        x = m_xCoords[idx];
        y = m_yCoords[idx];
    }
}

void Area2D::setCoord(int idx, int x, int y)
{
    if ((idx>=0)&&(idx<m_xCoords.size()))
    {
        m_xCoords[idx] = x;
        m_yCoords[idx] = y;
    }
}

bool Area2D::inside(int x, int y)
{
    // This code is (C)1998 Darel R. Finley. Permission is granted for
    // anyone to use this code, or any code derived from it, without
    // royalty, provided credit is given in the following form: "Some
    // code copyright Smokin' Software (TM) of Seabrook, Texas."

    int i, j=0;
    bool oddNODES=false;

    for (i=0; i<m_xCoords.size(); i++) {
        j++;
        if (j==m_xCoords.size()) j=0;
        if ( ((m_yCoords[i]<y) && (m_yCoords[j]>=y))||((m_yCoords[j]<y) && (m_yCoords[i]>=y)))
        {
            if (i!=j)
            {
                if (m_xCoords[i]+(y-m_yCoords[i])/(m_yCoords[j]-m_yCoords[i])*(m_xCoords[j]-m_xCoords[i])<x)
                    oddNODES=!oddNODES;
            }
        }
    }
    return oddNODES;
}

int Area2D::getSize()
{
    return (int)m_xCoords.size();
}


void Area2D::render()
{
    if (m_visible)
    {
        int i;
        glBegin(GL_QUADS);
        for (i=0; i<m_xCoords.size(); i++)
        {
            glColor3f(m_red[i], m_green[i], m_blue[i]);
            glVertex2i(m_xCoords[i],m_yCoords[i]);
        }
        glEnd();
    }
}

void Area2D::setVisible(bool flag)
{
    m_visible = flag;
}

void Area2D::setColor(int idx, float red, float green, float blue)
{
    if ((idx>=0)&&(idx<m_xCoords.size()))
    {
        m_red[idx] = red;
        m_green[idx] = green;
        m_blue[idx] = blue;
    }
}
