#include <ofutil/util_functions.h>

#include <sstream>

namespace ofutil {

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
}
