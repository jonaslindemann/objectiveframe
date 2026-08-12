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
 * Mirror is on its own tab and is not previewed - it adds geometry rather than
 * moving it, so there is nothing to restore from a baseline.
 */
class TransformWindow : public UiWindow {
public:
    /** Which tab is in front. */
    enum Mode {
        Move = 0,
        Scale = 1,
        Rotate = 2,
        Taper = 3,
        Mirror = 4
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

        int mirrorAxis{0};
        int mirrorOrigin{2};
        float weldTolerance{0.001f};
    };

private:
    FemViewWindow *m_view;
    Fields m_fields;

    /** Starts the session if needed, then pushes the current values. */
    void pushPreview();

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
