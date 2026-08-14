#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

/**
 * Interactive transform panel.
 *
 * Editing any field starts a preview session on the view, which captures the
 * affected nodes once and re-applies the transform from those captured
 * coordinates on every change. Apply makes the current preview permanent as a
 * single undo entry, Reset puts the nodes back.
 *
 * Mirror and Array are on their own tabs and are not previewed - they add
 * geometry rather than moving it, so there is nothing to restore from a
 * baseline. Both are driven by a button instead.
 */
class TransformWindow : public UiWindow {
public:
    /** Which tab is in front. */
    enum Mode {
        Move = 0,
        Scale = 1,
        Rotate = 2,
        Taper = 3,
        Smooth = 4,
        Mirror = 5,
        Array = 6
    };

    /**
     * The raw widget values.
     *
     * Plain types only - translating these into a transform description is the
     * .cpp's job, which keeps this header free of objframe types.
     */
    struct Fields {
        int mode{Move};
        int origin{1}; //!< Index into the panel's origin combo, not an Origin

        float translate[3]{0.0f, 0.0f, 0.0f};

        bool uniformScale{true};
        float scaleUniform{1.0f};
        float scale[3]{1.0f, 1.0f, 1.0f};

        int rotAxis{1};
        float rotAngle{0.0f};

        int taperAxis{1};
        float taperS0{1.0f};
        float taperS1{1.0f};

        int smoothIterations{10};
        float smoothLambda{0.5f};
        float smoothMu{-0.53f};
        bool smoothLengthWeighted{false};
        bool pinBC{true};
        bool pinLoaded{true};

        int mirrorAxis{0};
        int mirrorOrigin{2};

        // Shared by mirror and array - "coincident" should mean one thing on
        // both tabs.

        float weldTolerance{0.001f};

        int arrayKind{0};  //!< 0 linear, 1 polar, 2 grid
        int arrayCount{4}; //!< Instances including the original

        float arrayOffset[3]{1.0f, 0.0f, 0.0f};
        bool arraySpanStep{true}; //!< Offset counted in selection lengths

        int arrayAxis{1};
        float arrayTotalAngle{360.0f};
        bool arrayFullCircle{true};
        bool arrayRotateCopies{true};
        int arrayOrigin{0}; //!< Index into the panel's polar origin combo

        bool arrayCopyLoads{true};

        // Grid. The plane names its two axes in order, so xz means count/step
        // along x and count2/step2 along z.

        int arrayPlane{1}; //!< 0 xy, 1 xz, 2 yz
        int arrayCount2{3};
        float arrayStep1{5.0f};
        float arrayStep2{5.0f};

        /**
         * Grid keeps its own span flag, off by default.
         *
         * A grid usually repeats a planar frame both along itself and out of
         * its plane, and the selection has no extent in that second direction -
         * measuring the step against it would give zero. Sharing the linear
         * flag, which defaults on, would make that the first thing you hit.
         */
        bool arrayGridSpanStep{false};
    };

private:
    FemViewWindow *m_view;
    Fields m_fields;

    /** Starts the session if needed, then pushes the current values. */
    void pushPreview();

    /** The array tab, which is button driven rather than previewed. */
    void drawArrayTab();

    /** Returns the fields of the active tab to their neutral values. */
    void resetFields();

    /** True when the active tab would actually move anything. */
    bool hasChange() const;

public:
    TransformWindow(const std::string name);
    virtual ~TransformWindow();

    void setView(FemViewWindow *view);

    static std::shared_ptr<TransformWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<TransformWindow> TransformWindowPtr;

} // namespace ofui
