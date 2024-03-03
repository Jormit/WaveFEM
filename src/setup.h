#pragma once
#include <string>
#include <vector>

#include "geometry.h"

struct material_setup
{
	bool PEC;
	double ep;
	double mu;
	double tand;
};

struct setup
{
	setup(std::string filename);

	std::string model_file;
	std::vector<point_3d> port_centres;
	point_3d bounding_box_padding;
	bool pml_enable;
	point_3d pml_thickness;
	double simulation_wavenumber;
	std::vector<size_t> material_assignments;
	std::vector<material_setup> materials;
};