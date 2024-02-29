#pragma once
#include <string>
#include <Eigen/dense>
#include <complex>

#include "geometry.h"
#include "post.h"

namespace result_writer
{
	std::string complex_number(std::complex<double> num);
	void write_2d_field(std::string filename, structured_2d_field_data data);
	void write_3d_field(std::string filename, structured_3d_field_data data);
}
