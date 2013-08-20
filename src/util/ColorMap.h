#ifndef _CColorMap_h_
#define _CColorMap_h_

#include "Color.h"

#include <vector>
#include <fstream>
#include <string>

class CColorMap {
private:
    std::vector<CColor*> m_colors;
    bool m_reverseColors;
    double m_value;
    float m_currentColor[3];
    std::string m_progPath;
    std::string m_fileName;
    std::fstream m_mapFile;
    bool m_error;
public:
    CColorMap();
    virtual ~CColorMap();

    void open(const std::string& fname);
    bool open();
    void clear();
    void setPath(const std::string& path);

    void setFileName(const std::string& fileName);
    void setValue(double value);
    void setReverseColors(bool flag);
    void getColor(double value, float &r, float &g, float &b);
};

#endif
