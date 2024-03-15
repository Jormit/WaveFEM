#pragma once
#include <vector>
#include <unordered_map>

#include "geometry.h"

struct ports
{
	ports(std::vector<point_3d> points);

	void setup_port_nodes(std::vector<node>& nodes);
	void setup_port_faces_and_edges(std::unordered_map<size_t, int>& boundary_edge_map, std::unordered_map<size_t, int>& boundary_face_map);

	std::vector<point_3d> port_points;
	std::vector<int> entity_ids;
	std::vector<std::vector<tri>> elements;
	std::vector<rectangle> parametric_bounds;
};