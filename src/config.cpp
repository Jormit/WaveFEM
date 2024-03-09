#include <fstream>
#include <json.hpp>
#include <unordered_map>

#include "config.h"
#include "constants.h"

using json = nlohmann::json;

sim_config::sim_config(std::string filename)
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
	pml_enable = data["pml_enable"];
	simulation_wavenumber = static_cast<double>(data["frequency"]) * constants::freq2k;

	std::unordered_map<std::string, size_t> material_map;
	size_t i = 0;
	for (auto& [key, m] : data["material_definitions"].items())
	{
		material_map[key] = i++;
		if (m["PEC"])
		{
			material_config new_mat{ m["PEC"], 0, 0, 0 };
			materials.push_back(new_mat);
		}
		else
		{
			material_config new_mat{ m["PEC"], m["ep"], m["mu"], m["tand"] };
			materials.push_back(new_mat);
		}		
	}

	for (auto& m : data["material_assignments"])
	{
		material_assignments.push_back(material_map[m]);
	}
}