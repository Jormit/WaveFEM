#include "setup.h"
#include <fstream>
#include <sstream>

std::vector<std::vector<double>> setup::load_port_setup(std::string filename)
{
	std::vector<std::vector<double>> port_points;
	std::string line;
	std::ifstream file(filename);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			if (line[0] != '#')
			{
				std::istringstream stm(line);
				port_points.push_back({ std::istream_iterator<double>(stm), std::istream_iterator<double>() });
			}
		}
	}
	return port_points;
}
