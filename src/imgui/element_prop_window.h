#pragma once

#include "ui_window.h"

#include <VisFemBeam.h>

class ElementPropWindow : public UiWindow {
private:
	std::string m_name;
	VisFemBeam* m_beam;
public:
	ElementPropWindow(const std::string name);
	virtual ~ElementPropWindow();

	void setBeam(VisFemBeam* beam);

	static std::shared_ptr<ElementPropWindow> create(const std::string name);

protected:
	virtual void doDraw();
};

typedef std::shared_ptr<ElementPropWindow> ElementPropWindowPtr;

