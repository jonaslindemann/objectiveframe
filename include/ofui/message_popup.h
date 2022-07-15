#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include <ofui/popup_window.h>

namespace ofui {

class MessagePopup : public PopupWindow {
private:
	std::string m_message;
public:
	MessagePopup(const std::string name, bool modal);
	virtual ~MessagePopup();

	static std::shared_ptr<MessagePopup> create(const std::string name, bool modal=true);

	void setMessage(std::string message);

protected:

	virtual void doPopup();

};

typedef std::shared_ptr<MessagePopup> MessagePopupPtr;

}