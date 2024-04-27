#include <fstream>
#include <json.hpp>
#include <unordered_map>
#include <iostream>
#include <format>

#include "config.h"
#include "constants.h"

using json = nlohmann::json;

sim_config::sim_config(std::string filename)
{
	std::ifstream f;
	f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try 
	{
		f.open(filename);
	}
	catch (std::system_error& e) 
	{
		std::cerr << std::format("Could not open \"{}\"", filename) << std::endl;
		exit(1);
	}

	try
	{
		json data = json::parse(f);

		model_file = data["model_file"];
		for (auto& p : data["port_bounding_boxes"])
		{
			double xmin = static_cast<double>(p[0]);
			double xmax = static_cast<double>(p[1]);
			double ymin = static_cast<double>(p[2]);
			double ymax = static_cast<double>(p[3]);
			double zmin = static_cast<double>(p[4]);
			double zmax = static_cast<double>(p[5]);
			port_bounding_boxes.push_back({ xmin, ymin, zmin, xmax, ymax, zmax });
		}

		bounding_box_padding = static_cast<double>(data["bounding_box_padding"]);
		pml_thickness = static_cast<double>(data["pml_thickness"]);

		pml_enable = static_cast<bool>(data["pml_enable"]);

		simulation_wavenumber = static_cast<double>(data["frequency"]) * constants::freq2k;

		target_mesh_size =
			constants::c / static_cast<double>(data["frequency"]) / 
			static_cast<double>(data["target_elements_per_wavelength"]) * 1000;

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

		for (const std::string& m : data["material_assignments"])
		{
			if (material_map.contains(m))
			{
				material_assignments.push_back(material_map[m]);
			}
			else
			{
				std::cerr << std::format("Material \"{}\" is undefined!", m) << std::endl;
				exit(1);
			}
			
		}
	}

	catch (const json::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}
}