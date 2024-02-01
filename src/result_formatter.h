#include <string>
#include <Eigen/dense>
#include <complex>

#include "geometry.h"

namespace result_formatter
{
	std::string complex_number(std::complex<double> num);
	std::string field_2d_at_point(point_2d point, Eigen::Vector2d field);
	std::string field_3d_at_point(point_3d point, Eigen::Vector3cd field);
}
