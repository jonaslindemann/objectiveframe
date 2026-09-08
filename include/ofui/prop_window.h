#pragma once

#include <ofui/ui_window.h>

#include <vfem/beam.h>

class FemViewWindow;

namespace ofui {

class PropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Beam *m_beam;
    vfem::Node *m_node;
    ivf::Composite *m_selectedShapes;

    FemViewWindow *m_view;

    float m_beamRotation;
    float m_oldBeamRotation;
    int m_beamType;

    float m_nodePos[3];
    float m_nodeRot[3];
    float m_nodeDispl[3];
    float m_nodeMove[3];

    float m_nodeReactionForces[3];
    float m_nodeReactionMoments[3];

    // Set position group.
    //
    // Plain types only - the summary is read from the view in the .cpp, which
    // is the only place that may include an objframe header.

    bool m_setCoordAxis[3]; //!< Which axes Apply writes; the rest keep their values
    float m_setCoordValue[3];

    // What the affected nodes currently measure, for seeding the fields and
    // for telling "they all share this" from "these are spread".

    bool m_setCoordUniform[3];
    float m_setCoordLo[3];
    float m_setCoordHi[3];
    int m_setCoordCount;

    /**
     * Whether the summary needs re-reading from the model.
     *
     * Set when the selection changes rather than reading there directly, so a
     * paint select drag does not rebuild the model graph once per event. The
     * read happens at most once a frame, and only while the panel is open.
     */
    bool m_setCoordDirty;

public:
    PropWindow(const std::string name);
    virtual ~PropWindow();

    void setBeam(vfem::Beam *beam);
    void setSelectedShapes(ivf::Composite *selected);

    void setNode(vfem::Node *node);

    void setView(FemViewWindow *view);

    /**
     * Marks the seeded coordinate fields as stale.
     *
     * Called when the model geometry moves under an unchanged selection, so a
     * transform applied from elsewhere does not leave this panel quoting
     * coordinates the nodes no longer have.
     */
    void markCoordSummaryDirty();

    static std::shared_ptr<PropWindow> create(const std::string name);

protected:
    virtual void doDraw();

private:
    /** Re-reads the affected nodes and seeds the fields from them. */
    void updateCoordSummary();

    void drawSetCoord();
};

typedef std::shared_ptr<PropWindow> PropWindowPtr;

} // namespace ofui
