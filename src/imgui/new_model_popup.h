#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include <popup_window.h>

class NewModelPopup : public PopupWindow {
private:
	float m_modelSize;
	float m_nodeSize;
	float m_lineRadius;
	float m_loadSize;
public:
	NewModelPopup(const std::string name, bool modal);
	virtual ~NewModelPopup();

	static std::shared_ptr<NewModelPopup> create(const std::string name, bool modal=true);

	float modelSize();
	float nodeSize();
	float lineRadius();
	float loadSize();

protected:

	virtual void doPopup();

};

typedef std::shared_ptr<NewModelPopup> NewModelPopupPtr;

