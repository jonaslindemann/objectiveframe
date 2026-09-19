#pragma once

#include <string>

namespace ofui {

/**
 * Which interface the application presents.
 *
 * A mode is only ever a name for a preset - nothing downstream asks which mode
 * is in force, it asks whether a given feature is enabled. Adding a third
 * profile is then a row in applyPreset() rather than an edit at every call
 * site.
 */
enum class UiMode {
    Simple,
    Advanced
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
    BeamTypes,        ///< Choosing bar over beam when creating elements
    LoadDialogs,      ///< The node load and beam load property dialogs
    BcDialogs,        ///< The node boundary condition property dialog
    Materials,        ///< The material property dialog

    /**
     * The analytical half of the eigenmode panel - choosing how many modes to
     * compute, running it by hand, stepping through the modes, and the list of
     * eigenvalues as frequencies. What is left is the first mode, animated,
     * which is the question "is this thing a mechanism, and how does it fold".
     */
    EigenmodeDetails,

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
    static UiMode modeFromName(const std::string &name, UiMode defaultMode = UiMode::Advanced);

private:
    UiProfile();
    UiProfile(const UiProfile &) = delete;
    UiProfile &operator=(const UiProfile &) = delete;

    void applyPreset(UiMode mode);

    static UiProfile *m_this;

    UiMode m_mode{UiMode::Advanced};
    bool m_features[static_cast<int>(UiFeature::Count)];
    int m_revision{0};
};

} // namespace ofui
