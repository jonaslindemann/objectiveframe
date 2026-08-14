#pragma once

namespace ofui {

/**
 * The principal planes, as the array commands number them.
 *
 * A plane names its two axes in order, and every grid control follows that
 * order: an xz grid asks for x repeat, x step, z repeat, z step. Shared by the
 * transform panel and the grid popup so the two cannot disagree about which
 * field means which axis.
 */

/** Combo string, in plane order. */
inline const char *planeNames()
{
    return "XY\0XZ\0YZ\0";
}

/** Label of axis `which` (0 or 1) of `plane`. */
inline const char *planeAxisLabel(int plane, int which)
{
    static const char *labels[3][2] = {{"X", "Y"}, {"X", "Z"}, {"Y", "Z"}};

    if ((plane < 0) || (plane > 2) || (which < 0) || (which > 1))
        return "?";

    return labels[plane][which];
}

} // namespace ofui
