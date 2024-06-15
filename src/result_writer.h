#pragma once
#include <string>
#include <Eigen/dense>
#include <Eigen/sparse>
#include <complex>
#include <unsupported/Eigen/SparseExtra>

#include "geometry.h"
#include "post.h"

namespace result_writer
{
	std::string complex_number(std::complex<double> num);
	void write_structured_2d_field(std::string filename, const geo::structured_2d_field_data& data);
	void write_structured_3d_field(std::string filename, const geo::structured_3d_field_data& data);
	void write_unstructured_3d_field(std::string filename, const geo::unstructured_3d_field_data& data);
	void write_polar_2d_field_data(std::string filename, const geo::polar_2d_field_data& data);

	template <typename T>
	void write_mtx(std::string filename, T A)
	{
		Eigen::saveMarket<T>(A, filename + ".mtx");
	}
}
