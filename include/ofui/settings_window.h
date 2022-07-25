#pragma once

#include <ofui/ui_window.h>

class FemWidget;

namespace ofui {

class SettingsWindow : public UiWindow {
private:
	float m_size;
	float m_nodeSize;
	float m_lineRadius;
	float m_loadSize;
	float m_scaleFactor;
	bool m_lockScaleFactor;
	bool m_showNodeNumbers;

	FemWidget* m_widget;
public:
	SettingsWindow(const std::string name);
	virtual ~SettingsWindow();

	void setFemWidget(FemWidget* femWidget);

	void update();

	static std::shared_ptr<SettingsWindow> create(const std::string name);

protected:
	virtual void doDraw();
};

typedef std::shared_ptr<SettingsWindow> SettingsWindowPtr;

}