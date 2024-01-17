#include "setup.h"
#include <fstream>
#include <sstream>

std::vector<point> setup::load_port_setup(std::string filename)
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
	return port_points;
}
