#pragma once

#include <string>

namespace ofui {

/**
 * Which interface the application presents.
 *
 * A mode is only ever a name for a preset - nothing downstream asks which mode
 * is in force, it asks whether a given feature is enabled. Adding a profile is
 * then a row in applyPreset() rather than an edit at every call site.
 *
 * The two simple profiles share one reduced interface and differ only in what
 * an element is taken to be: bars carrying axial force, or beams with the full
 * set of sectional results.
 */
enum class UiMode {
    SimpleBar,
    SimpleBeam,
    Advanced
};

/**
 * Which element a profile creates while the beam-type buttons are hidden.
 *
 * The simple profiles take the choice away from the user, so the profile has
 * to make it instead - a bar profile that quietly created beams would show a
 * normal force for a model that is not carrying its load that way.
 */
enum class UiElementType {
    Beam,
    Bar
};

/**
 * A group of interface elements that a profile can switch off as a unit.
 *
 * Only features that something actually gates belong here - an entry with no
 * call site claims to hide something and does not. To gate a new group: add a
 * value, list it in applyPreset()'s simple-mode set, and tag the widgets with
 * it.
 *
 * UiFeature::None is the tag for anything unconditional, so an untagged widget
 * needs no special case.
 */
enum class UiFeature {
    None = 0,

    SelectionFilters, ///< The nodes/beams/all filter buttons on the main toolbar
    LoadDialogs,      ///< The node load and beam load property dialogs
    BcDialogs,        ///< The node boundary condition property dialog
    Materials,        ///< The material property dialog

    /**
     * The two element creation tools on the model toolbar, one feature each.
     *
     * A profile that leaves both on lets the user choose what to create; one
     * that leaves a single one on has made the choice, and
     * defaultElementType() says which way.
     */
    CreateBeamTool,
    CreateBarTool,

    /**
     * The analytical half of the eigenmode panel - choosing how many modes to
     * compute, running it by hand, stepping through the modes, and the list of
     * eigenvalues as frequencies. What is left is the first mode, animated,
     * which is the question "is this thing a mechanism, and how does it fold".
     */
    EigenmodeDetails,

    /**
     * Everything the result toolbar and the Results menu offer beyond normal
     * force: torsion, shear, moment and Navier. A bar carries axial force
     * only, so a bar profile has nothing to say about the rest and offering
     * them invites reading a result that is identically zero as a finding.
     */
    BeamResultTypes,

    Count
};

/**
 * The set of interface features currently enabled.
 *
 * A singleton in the manner of ofutil::AppSettings, so that any panel can gate
 * its own contents without having the profile threaded through its create().
 * It holds no persistence of its own - the application reads the stored mode at
 * startup and writes it back when it changes.
 */
class UiProfile {
public:
    static UiProfile *instance();

    void setMode(UiMode mode);
    UiMode mode() const;

    bool has(UiFeature feature) const;

    /**
     * The element the current preset creates, for the profiles that do not let
     * the user pick. Advanced returns Beam, which is what an element is unless
     * something says otherwise - both creation tools are on screen there, and
     * picking one overrides this immediately.
     */
    UiElementType defaultElementType() const;

    /**
     * Overrides a single feature, leaving the rest of the preset alone.
     *
     * The mode name is kept as-is: it says which preset was last applied, and a
     * one-off override does not make it a different preset.
     */
    void setFeature(UiFeature feature, bool flag);

    /**
     * Bumped whenever the enabled set changes.
     *
     * Lets the view notice a change it did not make itself - the start window
     * and the menu both go straight to the profile, and the view picks the
     * change up on the next frame rather than every caller having to remember
     * to notify it.
     */
    int revision() const;

    static std::string modeName(UiMode mode);
    static UiMode modeFromName(const std::string &name, UiMode defaultMode = UiMode::SimpleBar);

private:
    UiProfile();
    UiProfile(const UiProfile &) = delete;
    UiProfile &operator=(const UiProfile &) = delete;

    void applyPreset(UiMode mode);

    static UiProfile *m_this;

    UiMode m_mode{UiMode::SimpleBar};
    UiElementType m_elementType{UiElementType::Bar};
    bool m_features[static_cast<int>(UiFeature::Count)];
    int m_revision{0};
};

} // namespace ofui
