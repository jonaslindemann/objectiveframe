#include "ColorMap.h"

#include <iostream>
#include <memory>

ColorMap::ColorMap()
{
    m_reverseColors = false;
    m_progPath = "";
    m_fileName = "";
    m_error = false;
}

ColorMap::~ColorMap()
{
    this->clear();
}

std::shared_ptr<ColorMap> ColorMap::create()
{
    return std::make_shared<ColorMap>();
}

void ColorMap::open(const std::string &fname)
{
    using namespace std;

    int nColors;
    float r, g, b;

    this->setFileName(fname);

    m_error = false;

    // Clear previous color map

    this->clear();

    // Open color map file

    std::string fileName = "";

    if (m_progPath != "")
        fileName = m_progPath;

    fileName = fileName + "/" + fname;

    m_mapFile.open(fileName.c_str(), ios::in);
    if (m_mapFile.fail()) {
        cout << "Could not open colormap file:" << endl;
        cout << "\t" << fileName << endl;
        m_error = true;
        return;
    }

    // Read colors

    m_mapFile >> nColors;

    for (auto i = 0; i < nColors; i++) {
        m_mapFile >> r >> g >> b;
        auto color = ColorEntry::create();
        color->setColor(r, g, b, 1.0f);
        m_colors.push_back(color);
    }

    // Close file

    m_mapFile.close();
}

void ColorMap::clear()
{
    m_colors.clear();
}

void ColorMap::getColor(double value, float &r, float &g, float &b)
{
    if (m_colors.size() > 0) {
        double clampedValue;
        int colorIndex;
        float a;

        r = g = b = 1.0;

        // Clamp value

        clampedValue = value;

        if (value > 1)
            clampedValue = 1.0;

        if (value < 0)
            clampedValue = 0.0;

        if (m_reverseColors)
            clampedValue = 1 - clampedValue;

        colorIndex = (int)((double)(m_colors.size() - 1) * clampedValue);

        auto color = m_colors[colorIndex];
        color->getColor(r, g, b, a);
    }
    else
        r = g = b = 1.0;
}

void ColorMap::setReverseColors(bool flag)
{
    m_reverseColors = flag;
}

void ColorMap::setValue(double value)
{
    m_value = value;
    this->getColor(m_value, m_currentColor[0], m_currentColor[1], m_currentColor[2]);
}

void ColorMap::setPath(const std::string &path)
{
    m_progPath = path;
}

void ColorMap::setFileName(const std::string &fileName)
{
    m_fileName = fileName;
}

bool ColorMap::open()
{
    if (m_fileName != "") {
        this->open(m_fileName);
        return m_error;
    }
    else
        return false;
}
