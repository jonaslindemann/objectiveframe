#pragma once

#include <memory>

/**
 * Color class
 */
class ColorEntry {
private:
    float m_color[4];

public:
    /** Color class constructor. */
    ColorEntry();

    /** Color class destructor. */
    virtual ~ColorEntry();

    static std::shared_ptr<ColorEntry> create();

    // Get/set methods

    /** Sets color value. */
    void setColor(float red, float green, float blue, float alpha);
    void getColor(float &r, float &g, float &b, float &a);

    /** Set color alpha value. */
    void setAlpha(float alpha);

    /** Assigns array the current color value. */
    void getColor(float *array);
};

typedef std::shared_ptr<ColorEntry> ColorEntryPtr;
