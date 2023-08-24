#pragma once

#include "Color.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class ColorMap {
private:
    std::vector<ColorEntryPtr> m_colors;
    bool m_reverseColors;
    double m_value;
    float m_currentColor[3];
    std::string m_progPathStr;
    std::filesystem::path m_progPath;

    std::string m_fileName;
    std::fstream m_mapFile;
    bool m_error;

public:
    ColorMap();
    virtual ~ColorMap();

    static std::shared_ptr<ColorMap> create();

    void open(const std::string &fname);
    bool open();
    void clear();
    void setPath(const std::string &path);

    void setFileName(const std::string &fileName);
    void setValue(double value);
    void setReverseColors(bool flag);
    void getColor(double value, float &r, float &g, float &b);
};

typedef std::shared_ptr<ColorMap> ColorMapPtr;
