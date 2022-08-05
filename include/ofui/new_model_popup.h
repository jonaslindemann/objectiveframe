#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include <ofui/popup_window.h>

namespace ofui
{

class NewModelPopup : public PopupWindow
{
private:
    float m_modelSize;
    float m_nodeSize;
    float m_lineRadius;
    float m_loadSize;

public:
    NewModelPopup(const std::string name, bool modal);
    virtual ~NewModelPopup();

    static std::shared_ptr<NewModelPopup> create(const std::string name, bool modal = true);

    float modelSize();
    float nodeSize();
    float lineRadius();
    float loadSize();

    void modelSize(float value);
    void nodeSize(float value);
    void lineRadius(float value);
    void loadSize(float value);

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<NewModelPopup> NewModelPopupPtr;

}
