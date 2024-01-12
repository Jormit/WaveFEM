#pragma once
#include <vector>
#include "geometry.h"
#include "../include/gmsh.h_cwrap"

namespace mesher_interface {
	// Initializes GMSH.
	void initialize();

	// Loads file of BREP, STEP or IGES format into GMSH environment.
	// Returns: id of model
	int import_model(const std::string& filename);

	// Perform meshing.
	void mesh_model(int mesh_size_min, int mesh_size_max);

	// Loads gmsh viwer.
	void view_model();

	// Returns bounding box of entire model
	box get_bounding_box();

	// Adds box to model
	// Returns: id of box
	int add_box(box b);

	// Subtracts volume with id2 from id1
	int subtract(int id1, int id2);

	// Get node map
	std::unordered_map<int, int> get_node_map();

	// Get nodes
	std::vector<node> get_all_nodes(std::unordered_map<int, int> node_map);

	// Get elements (tetrahedrons)
	std::vector<tet> get_volume_elems(std::unordered_map<int, int> node_map);

	// Get surface ids by comparing center of mass
	std::vector<int> get_surface_ids_from_coms(std::vector<std::vector<double>> coms);

	// Get Surface elements (triangles)
	std::vector<std::vector<tri>> get_surface_elems_by_id(std::vector<int> ids, std::unordered_map<int, int> node_map);

}


