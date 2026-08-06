#pragma once

#include <ofui/ui_window.h>

namespace ofui {

class CoordWindow : public UiWindow {
private:
    std::string m_name;
    double m_coord[3];
    float m_contentWidth;

    void drawCoord(const char *label, double value);

public:
    CoordWindow(const std::string name);
    virtual ~CoordWindow();

    void setCoord(double x, double y, double z);
    void coord(double &x, double &y, double &z);

    void setContentWidth(float width);
    float contentWidth() const;

    static std::shared_ptr<CoordWindow> create(const std::string name);

protected:
    virtual void doPreDraw();
    virtual void doDraw();
};

typedef std::shared_ptr<CoordWindow> CoordWindowPtr;

} // namespace ofui
