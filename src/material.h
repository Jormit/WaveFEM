#pragma once
#include <Eigen/dense>
#include <vector>

#include "geometry.h"

const size_t FREE_SPACE = 0;
const size_t PML_X = 1;
const size_t PML_Y = 2;
const size_t PML_Z = 3;

struct material
{
	Eigen::Matrix3cd permeability;
	Eigen::Matrix3cd permittivity;
};

material default_material();

std::vector<material> generate_base_material_set();
void label_pml_elements(std::vector<tet>& elems, box non_pml_boundary, const std::vector<node>& nodes);