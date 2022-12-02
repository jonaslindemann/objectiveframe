#ifndef _CIvfArea2D_h_
#define _CIvfArea2D_h_

#include <ivf/Base.h>
#include <ivf/GL.h>

IvfSmartPointer(Area2D);

class Area2D : public ivf::Base
{
private:
    std::vector<int> m_xCoords;
    std::vector<int> m_yCoords;
    std::vector<float> m_red;
    std::vector<float> m_green;
    std::vector<float> m_blue;
    bool m_visible;

public:
    Area2D();
    virtual ~Area2D();

    IvfClassInfo("Area2D", ivf::Base);
    IvfStdFactory(Area2D);

    void add(int x, int y);
    void clear();
    int getSize();

    void setCoord(int idx, int x, int y);
    void getCoord(int idx, int& x, int& y);
    void setColor(int idx, float red, float green, float blue);
    void setVisible(bool flag);

    bool inside(int x, int y);
    void render();
};

#endif
