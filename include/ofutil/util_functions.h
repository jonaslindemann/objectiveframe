#pragma once

#include <string>

namespace ofutil
{

std::string to_string(float value);
std::string to_string(double value);
std::string to_string(int value);
std::string to_string(size_t value);

std::string to_coord_string(double x, double y);
std::string to_coord_string(double x, double y, double z);

float to_float(std::string str);
int to_int(std::string str);
int run_process(std::string cmd);

}
