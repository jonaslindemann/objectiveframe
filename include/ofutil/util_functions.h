#pragma once

#include <string>

namespace ofutil {
	
	std::string to_string(float value);
	std::string to_string(double value);
	std::string to_string(int value);

	std::string to_coord_string(double x, double y);
	std::string to_coord_string(double x, double y, double z);

}

