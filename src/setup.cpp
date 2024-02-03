#include <fstream>
#include <json.hpp>

#include "setup.h"
#include "constants.h"

using json = nlohmann::json;

setup::setup(std::string filename)
{
	std::ifstream f(filename);
	json data = json::parse(f);

	model_file = data["model_file"];
	for (auto& p : data["port_centres"])
	{
		double x = p[0];
		double y = p[1];
		double z = p[2];
		port_centres.push_back({ x,y,z });
	}

	bounding_box_padding = { data["bounding_box_padding"][0], data["bounding_box_padding"][1], data["bounding_box_padding"][2] };
	pml_thickness = { data["pml_thickness"][0], data["pml_thickness"][1], data["pml_thickness"][2] };
	simulation_wavenumber = static_cast<double>(data["frequency"]) * constants::freq2k;
}