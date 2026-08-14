#pragma once

#include <memory>
#include <string>

#include <imgui.h>

#include <ofui/popup_window.h>

namespace ofui {

/**
 * Grid array dialog for one principal plane.
 *
 * The short way to the common request - repeat the selection across a plane.
 * The plane is chosen from the menu that opens the dialog, so the dialog itself
 * only asks for the two repeats and the two steps, named after the axes of that
 * plane.
 *
 * It carries no logic of its own: FemViewWindow reads the fields back and calls
 * planeArraySelection(), the same entry point the transform panel's Grid mode
 * uses, so the two cannot behave differently.
 */
class ArrayGridPopup : public PopupWindow {
private:
    int m_plane;
    int m_count1;
    int m_count2;
    float m_step1;
    float m_step2;
    bool m_spanStep;
    bool m_copyLoads;
    float m_weldTolerance;

public:
    ArrayGridPopup(const std::string name, bool modal);
    virtual ~ArrayGridPopup();

    static std::shared_ptr<ArrayGridPopup> create(const std::string name, bool modal = true);

    int plane();
    int count1();
    int count2();
    float step1();
    float step2();
    bool spanStep();
    bool copyLoads();
    float weldTolerance();

    void plane(int value);
    void count1(int value);
    void count2(int value);
    void step1(float value);
    void step2(float value);
    void spanStep(bool value);
    void copyLoads(bool value);
    void weldTolerance(float value);

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<ArrayGridPopup> ArrayGridPopupPtr;

} // namespace ofui
