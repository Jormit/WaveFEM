#pragma once
#include <vector>
#include <string>
#include "geometry.h"

class ports
{
public:
	ports(std::vector<point_3d> points);

	void setup_port_nodes(std::vector<node>& nodes);

	friend class sim;

private:
	std::vector<point_3d> port_points;
	std::vector<int> entity_ids;
	std::vector<std::vector<tri>> elements;
	std::vector<rectangle> bounds;
};