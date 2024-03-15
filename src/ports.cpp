#include "ports.h"
#include "mesher_interface.h"

#include <fstream>
#include <sstream>

ports::ports(std::vector<point_3d> points)
{
	port_points = points;
	entity_ids = mesher_interface::get_surface_from_com(port_points);
	elements = mesher_interface::get_surface_elems(entity_ids);
	parametric_bounds = mesher_interface::get_surface_parametric_bounds(entity_ids);
	bounding_boxes = mesher_interface::get_bounding_box(2, entity_ids);
}

void ports::setup_port_nodes(std::vector<node>& nodes)
{
	mesher_interface::parameterize_surface_nodes(nodes, entity_ids, elements);
}

void ports::setup_port_faces_and_edges(std::unordered_map<size_t, int>& boundary_edge_map, std::unordered_map<size_t, int>& boundary_face_map)
{
	for (int i = 0; i < entity_ids.size(); i++)
	{
		std::unordered_set<size_t> port_edges;
		std::unordered_set<size_t> port_faces;
		std::tie(port_edges, port_faces) = mesher_interface::get_surface_edges_and_faces(entity_ids[i]);
		for (auto edge : port_edges)
		{
			boundary_edge_map[edge] = i + 1;
		}

		for (auto face : port_faces)
		{
			boundary_face_map[face] = i + 1;
		}
	}

	auto port_outer_boundaries = mesher_interface::get_surface_boundary_edges(entity_ids);
	for (auto edge : port_outer_boundaries)
	{
		boundary_edge_map[edge] = PORT_OUTER_BOUNDARY;
	}
}
