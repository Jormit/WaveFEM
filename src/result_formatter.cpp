#include "result_formatter.h"

#include <format>

std::string result_formatter::complex_number(std::complex<double> num)
{
	if (num.imag() > 0)
	{
		return std::format("{}+{}i", num.real(), num.imag());
	}
	return std::format("{}{}i", num.real(), num.imag());
}

std::string result_formatter::field_2d_at_point(point_2d point, Eigen::Vector2d field)
{
	return std::format("{} {} {} {}\n", point.u, point.v, field(0), field(1));
}

std::string result_formatter::field_2d_at_point(point_2d point, Eigen::Vector2cd field)
{
	return std::format("{} {} {} {}\n", point.u, point.v, complex_number(field(0)), complex_number(field(1)));
}

std::string result_formatter::field_3d_at_point(point_3d point, Eigen::Vector3cd field)
{
	return std::format("{} {} {} {} {} {}\n", point.x, point.y, point.z,
		complex_number(field(0)), complex_number(field(1)), complex_number(field(2)));
}