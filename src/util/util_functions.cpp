#include <ofutil/util_functions.h>

#include <sstream>

namespace ofutil
{

std::string to_string(float value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(double value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(int value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_string(size_t value)
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

std::string to_coord_string(double x, double y)
{
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ")";
    return ss.str();
}

std::string to_coord_string(double x, double y, double z)
{
    std::ostringstream ss;
    ss << "(" << x << ", " << y << ", " << z << ")";
    return ss.str();
}
float to_float(std::string str)
{
    std::stringstream ss(str);

    float f;
    if (ss >> f)
        return f;
    else
        return 0.0;
}
int to_int(std::string str)
{
    std::stringstream ss(str);

    int f;
    if (ss >> f)
        return f;
    else
        return -1;
}
}
