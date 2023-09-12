#include <ofsolve/solver_interface.h>

#include <sstream>

std::string float2str(double value)
{
    std::stringstream coordStream;
    coordStream << std::fixed << std::setw(10) << std::setprecision(2) << value;
    return coordStream.str();
}
