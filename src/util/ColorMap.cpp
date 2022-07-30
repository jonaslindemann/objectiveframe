#include "ColorMap.h"

#include <iostream>

CColorMap::CColorMap()
{
    m_reverseColors = false;
    m_progPath = "";
    m_fileName = "";
    m_error = false;
}

CColorMap::~CColorMap()
{
    this->clear();
}

void CColorMap::open(const std::string& fname)
{
    using namespace std;

    int nColors;
    int i;
    float r, g, b;
    CColor* color;

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
    if (m_mapFile.fail())
    {
        cout << "Could not open colormap file:" << endl;
        cout << "\t" << fileName << endl;
        m_error = true;
        return;
    }

    // Read colors

    m_mapFile >> nColors;

    for (i = 0; i < nColors; i++)
    {
        m_mapFile >> r >> g >> b;
        color = new CColor();
        color->setColor(r, g, b, 1.0f);
        m_colors.push_back(color);
    }

    // Close file

    m_mapFile.close();
}

void CColorMap::clear()
{
    unsigned int i;

    for (i = 0; i < m_colors.size(); i++)
        delete m_colors[i];

    m_colors.clear();
}

void CColorMap::getColor(double value, float& r, float& g, float& b)
{
    if (m_colors.size() > 0)
    {
        double clampedValue;
        int colorIndex;
        CColor* color;
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

        color = m_colors[colorIndex];
        color->getColor(r, g, b, a);
    }
    else
        r = g = b = 1.0;
}

void CColorMap::setReverseColors(bool flag)
{
    m_reverseColors = flag;
}

void CColorMap::setValue(double value)
{
    m_value = value;
    this->getColor(m_value, m_currentColor[0], m_currentColor[1], m_currentColor[2]);
}

void CColorMap::setPath(const std::string& path)
{
    m_progPath = path;
}

void CColorMap::setFileName(const std::string& fileName)
{
    m_fileName = fileName;
}

bool CColorMap::open()
{
    if (m_fileName != "")
    {
        this->open(m_fileName);
        return m_error;
    }
    else
        return false;
}
