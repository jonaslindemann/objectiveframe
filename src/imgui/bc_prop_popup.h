#pragma once

#include <memory>
#include <string>
#include <vector>
#include <array>

#include <imgui.h>

#include <popup_window.h>

class FemWidget;

class BCPropPopup : public PopupWindow {
private:
	FemWidget* m_widget;
	std::array<char, 255> m_nameArr;
	int m_color;
	bool m_prescribedDofs[6];
	double m_prescribedValues[6];
public:
	BCPropPopup(const std::string name, bool modal);
	virtual ~BCPropPopup();

	static std::shared_ptr<BCPropPopup> create(const std::string name, bool modal=true);

	void setFemWidget(FemWidget* widget);

	void update();

protected:

	virtual void doPopup();

};

typedef std::shared_ptr<BCPropPopup> BCPropPopupPtr;


