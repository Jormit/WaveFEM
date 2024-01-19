#pragma once
#include <vector>
#include <string>
#include "geometry.h"

class ports
{
public:
	ports(std::string filename);
	ports(std::vector<point> points);

	friend class sim;

private:
	std::vector<point> port_points;
	std::vector<int> entity_ids;
	std::vector<std::vector<tri>> elements;
	std::vector<dimensions> dimensions;
};