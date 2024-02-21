#pragma once
#include <string>
#include <Eigen/dense>
#include <complex>

#include "geometry.h"

namespace result_writer
{
	std::string complex_number(std::complex<double> num);
	void write_2d_solution(std::string filename, Eigen::MatrixX2d, Eigen::MatrixX2cd);
	void write_3d_solution(std::string filename, Eigen::MatrixX3d, Eigen::MatrixX3cd);
}
