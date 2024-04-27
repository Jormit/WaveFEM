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
	std::vector<box> port_bounding_boxes;
	double bounding_box_padding;
	bool pml_enable;
	double pml_thickness;
	double simulation_wavenumber;
	double target_mesh_size;
	std::vector<size_t> material_assignments;
	std::vector<material_config> materials;
};