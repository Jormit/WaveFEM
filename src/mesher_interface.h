#pragma once
#include <vector>
#include "geometry.h"
#include <gmsh/gmsh.h_cwrap>

namespace mesher_interface {
	void initialize();

	// Loads file of BREP, STEP or IGES format into GMSH environment.
	int import_model(const std::string& filename);

	void mesh_model(int mesh_size_min, int mesh_size_max);

	void view_model();

	box get_bounding_box();
	box get_bounding_box(size_t dim, size_t tag);
	std::vector<box> get_bounding_box(size_t dim, std::vector<size_t> tags);

	int add_box(box b);

	int subtract(int id1, int id2);

	// Vector is indexed by node number - 1 !!!!!
	std::vector<node> get_all_nodes();

	std::vector<tet> get_volume_elems();

	int get_surface_from_com(point);
	std::vector<int> get_surface_from_com(std::vector<point> coms);

	std::vector<std::vector<tri>> get_surface_elems_by_ids(std::vector<int> ids);

	size_t get_element_by_coordinate(point points, int dim);
	std::vector<size_t> get_elements_by_coordinate(std::vector<point> points, int dim);
}


