#ifndef _CColor_h_
#define _CColor_h_

/**
 * Color class
 */
class CColor {
private:
    float m_color[4];
public:
    /** Color class constructor. */
    CColor();

    /** Color class destructor. */
    virtual ~CColor();

    // Get/set methods

    /** Sets color value. */
    void setColor(float red, float green, float blue, float alpha);
    void getColor(float &r, float &g, float &b, float &a);

    /** Set color alpha value. */
    void setAlpha(float alpha);

    /** Assigns array the current color value. */
    void getColor(float* array);
};

#endif

