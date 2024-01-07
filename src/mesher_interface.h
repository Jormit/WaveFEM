#pragma once
#include <vector>
#include "geometry.h"

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

	// Get nodes
	std::vector<node> get_nodes();

	// Get elements (tetrahedrons).
	std::vector<tet> get_elems();

}


