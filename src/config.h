#pragma once
#include <string>
#include <vector>

#include "geometry.h"

struct material_config
{
	bool PEC;
	double ep;
	double mu;
	double tand;
};

struct sim_config
{
	sim_config(std::string filename);

	std::string model_file;
	std::vector<point_3d> port_centres;
	point_3d bounding_box_padding;
	bool pml_enable;
	point_3d pml_thickness;
	double simulation_wavenumber;
	double target_mesh_size;
	std::vector<size_t> material_assignments;
	std::vector<material_config> materials;
};