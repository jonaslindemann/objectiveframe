#pragma once

#include "ui_window.h"

class CoordWindow : public UiWindow {
private:
	std::string m_name;
	double m_coord[3];
public:
	CoordWindow(const std::string name);
	virtual ~CoordWindow();

	void setCoord(double x, double y, double z);
	void coord(double& x, double& y, double& z);

	static std::shared_ptr<CoordWindow> create(const std::string name);

protected:
	virtual void doPreDraw();
	virtual void doDraw();
};

typedef std::shared_ptr<CoordWindow> CoordWindowPtr;

