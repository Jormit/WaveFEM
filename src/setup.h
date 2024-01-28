#pragma once
#include <string>
#include <vector>

#include "geometry.h"

struct setup
{
	setup(std::string filename);

	std::string model_file;
	std::vector<point_3d> port_centres;
	point_3d bounding_box_padding;
	point_3d pml_thickness;
	double simulation_wavenumber;
};