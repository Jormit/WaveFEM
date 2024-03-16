#include "ports.h"
#include "mesher_interface.h"
#include "material.h"

#include <fstream>
#include <sstream>

std::vector<box> ports::get_port_bounding_boxes(std::vector<point_3d> points)
{
	auto ids = mesher_interface::get_surface_from_com(points);
	return mesher_interface::get_bounding_box(2, ids);
}

ports::ports(std::vector<box> boxes, std::unordered_map<size_t, size_t> volume_material_map)
{
	entity_ids = mesher_interface::get_entities_in_bounding_box(2, boxes);
	dummy_ids = mesher_interface::fuse_surfaces(entity_ids, false);
	parametric_bounds = mesher_interface::get_surface_parametric_bounds(dummy_ids);
	elements = {};
	is_TEM = true;

	for (const auto & port_ids : entity_ids)
	{
		elements.push_back({});
		auto local_elements = mesher_interface::get_surface_elems(port_ids);
		auto parent_ids = mesher_interface::get_surface_parent(port_ids);
		for (size_t i = 0; i < local_elements.size(); i++)
		{
			if (volume_material_map.contains(parent_ids[i]))
			{
				mat::label_elems(local_elements[i], volume_material_map[parent_ids[i]]);
			}

			elements.back().insert(elements.back().end(), local_elements[i].begin(), local_elements[i].end());
		}
	}
}

void ports::setup_port_nodes(std::vector<node>& nodes)
{
	mesher_interface::parameterize_surface_nodes(nodes, dummy_ids, elements);
}

void ports::setup_port_faces_and_edges(std::unordered_map<size_t, int>& boundary_edge_map, std::unordered_map<size_t, int>& boundary_face_map)
{
	for (int i = 0; i < entity_ids.size(); i++)
	{
		for (auto id : entity_ids[i])
		{
			std::unordered_set<size_t> port_edges;
			std::unordered_set<size_t> port_faces;
			std::tie(port_edges, port_faces) = mesher_interface::get_surface_edges_and_faces(id);
			for (auto edge : port_edges)
			{
				boundary_edge_map[edge] = i + 1;
			}

			for (auto face : port_faces)
			{
				boundary_face_map[face] = i + 1;
			}
		}		

		auto port_outer_boundaries = mesher_interface::get_surface_boundary_edges(entity_ids[i]);
		for (auto edge : port_outer_boundaries)
		{
			boundary_edge_map[edge] = PORT_OUTER_BOUNDARY;
		}
	}
}
