#pragma once
#include <Eigen/dense>
#include <vector>

#include "geometry.h"

struct material
{
	material(std::complex<double> permittivity, std::complex<double> permeability);
	material(Eigen::Matrix3cd permittivity, Eigen::Matrix3cd permeability);
	Eigen::Matrix3cd permittivity;
	Eigen::Matrix3cd permeability;	
};

namespace mat
{
	const size_t FREE_SPACE = 0;
	const size_t PML_X = 1;
	const size_t PML_Y = 2;
	const size_t PML_Z = 3;
	const size_t PML_XY = 4;
	const size_t PML_XZ = 5;
	const size_t PML_YZ = 6;
	const size_t PML_XYZ = 7;

	material vacuum();
	std::vector<material> generate_base_set();
	void label_elems(std::vector<tet>& elems, size_t material);
	material pml(std::complex<double> sx, std::complex<double> sy, std::complex<double> sz);
}