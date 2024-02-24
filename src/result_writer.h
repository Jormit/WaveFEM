#pragma once
#include <string>
#include <Eigen/dense>
#include <complex>

#include "geometry.h"

namespace result_writer
{
	std::string complex_number(std::complex<double> num);
	void write_2d_field(std::string filename, structured_grid_2d grid, Eigen::MatrixX2cd);
	void write_3d_field(std::string filename, structured_grid_3d grid, Eigen::MatrixX3cd);
}
