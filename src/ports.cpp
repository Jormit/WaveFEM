#include "ports.h"
#include "mesher_interface.h"
#include <fstream>
#include <sstream>

ports::ports(std::string filename)
{
	std::vector<point> port_points;
	std::string line;
	std::ifstream file(filename);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (line[0] != '#')
			{
				std::istringstream stm(line);
				std::vector<double> temp({ std::istream_iterator<double>(stm), std::istream_iterator<double>() });
				port_points.push_back({ temp[0], temp[1], temp[2] });
			}
		}
	}
	this->port_points = port_points;
	entity_ids = mesher_interface::get_surface_from_com(port_points);
	elements = mesher_interface::get_surface_elems_by_id(entity_ids);
	dimensions = mesher_interface::get_surface_dimensions(entity_ids);
}

ports::ports(std::vector<point> points)
{
	port_points = points;
	entity_ids = mesher_interface::get_surface_from_com(port_points);
	elements = mesher_interface::get_surface_elems_by_id(entity_ids);
	dimensions = mesher_interface::get_surface_dimensions(entity_ids);
}