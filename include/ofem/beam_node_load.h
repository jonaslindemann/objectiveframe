#pragma once

#include <ofem/node_load.h>

#include <string>

namespace ofem {

SmartPointer(BeamNodeLoad);

class BeamNodeLoad : public NodeLoad {
private:
    int m_color;
    std::string m_name;
    double m_scale;

public:
    BeamNodeLoad();
    virtual ~BeamNodeLoad();

    ClassInfo("BeamNodeLoad", NodeLoad);
    StdFactory(BeamNodeLoad);

    void setColor(int color);
    int getColor();

    void setName(const std::string &name);
    const std::string getName();

    /**
     * A name describing what the load is, derived from its own direction and
     * magnitude.
     *
     * Quick load creation makes loads without asking the user to name them, and
     * an unnamed load is useless in the load list and the load mixer, where the
     * name is all there is to tell one slider from another. The name is
     * therefore the load: an axis aligned load reads "Fy -5", the sign folded
     * into the value so it matches how the user thinks of it, and anything
     * else keeps its magnitude and spells out the direction, "F 5 (0.6, -0.8,
     * 0)".
     *
     * Loads that coalesce share a direction and a magnitude, so they also share
     * this name - which is what makes it a usable identity rather than a label
     * that happens to be attached.
     *
     * \param dx,dy,dz direction, not necessarily normalised
     * \param value magnitude
     */
    static std::string autoName(double dx, double dy, double dz, double value);

    /**
     * Does this load already describe the given direction and magnitude?
     *
     * The test quick load creation uses to decide whether to join an existing
     * load or make a new one. The direction is compared normalised and the
     * magnitude relative to its own size, so the same answer comes back whether
     * the model is worked in newtons or meganewtons.
     */
    bool matches(double dx, double dy, double dz, double value);

    void setScale(double scale);
    double getScale();

    virtual void readFromStream(std::istream &in) override;
    virtual void saveToStream(std::ostream &out) override;
};
} // namespace ofem
