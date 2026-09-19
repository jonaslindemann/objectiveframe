#pragma once

#include <ofem/node_bc.h>

#include <string>

namespace ofem {

SmartPointer(BeamNodeBC);

class BeamNodeBC : public NodeBC {
private:
    int m_color;
    std::string m_name;

public:
    BeamNodeBC();
    virtual ~BeamNodeBC();

    ClassInfo("BeamNodeBC", NodeBC);
    StdFactory(BeamNodeBC);

    /**
     * The standard supports the application offers ready made.
     *
     * These are the patterns a user picks off a toolbar rather than dialling in
     * dof by dof. Each one lives in the model as a single named, read only BC
     * that every node using that support joins, so the BC list stays the same
     * length however many nodes are supported.
     *
     * The list is here rather than in the UI because it also governs
     * persistence: readFromStream() marks a BC read only when its name is one
     * of these, which is the only way a default survives a save/load round trip
     * (see isDefaultName()).
     */
    enum class DefaultKind {
        Fixed,   //!< All six dofs fixed - a fully built in support
        Pinned,  //!< All three translations fixed, free to rotate
        RollerX, //!< Free to translate along x, held in y and z
        RollerY, //!< Free to translate along y, held in x and z
        RollerZ  //!< Free to translate along z, held in x and y
    };

    /** The canonical, persisted name of a default support. */
    static const char *defaultName(DefaultKind kind);

    /**
     * Is this the name of one of the default supports?
     *
     * The names are the identity of a default - nothing else about a BC is
     * written to file that would distinguish one - so this is what
     * readFromStream() consults to restore the read only flag.
     */
    static bool isDefaultName(const std::string &name);

    /** Sets the dof pattern (and the name) of one of the default supports. */
    void applyDefault(DefaultKind kind);

    int getColor();
    void setColor(int color);
    const std::string getName();
    void setName(const std::string &name);

    void readFromStream(std::istream &in) override;
    void saveToStream(std::ostream &out) override;
};
} // namespace ofem
