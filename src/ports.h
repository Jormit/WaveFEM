#pragma once
#include <vector>
#include <unordered_map>

#include "geometry.h"

struct ports
{
	ports(std::vector<geo::box> boxes, std::unordered_map<size_t, size_t> volume_material_map);
	void setup_port_nodes_faces_edges(std::vector<geo::node>& nodes, std::unordered_map<size_t,
		int>& boundary_edge_map, std::unordered_map<size_t, int>& boundary_face_map);
	size_t num_ports() const;
	
	std::vector<std::vector<int>> entity_ids;
	std::vector<int> dummy_ids;
	std::vector<std::vector<geo::tri>> elements;
	std::vector<geo::rectangle> parametric_bounds;
};