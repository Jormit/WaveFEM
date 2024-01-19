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
	box get_bounding_box(int dim, size_t tag);
	std::vector<box> get_bounding_box(int dim, std::vector<size_t> tags);

	int add_box(box b);

	int subtract(int id1, int id2);

	// Vector is indexed by node number - 1 !!!!!
	std::vector<node> get_all_nodes();

	std::vector<tet> get_all_volume_elems();

	int get_surface_from_com(point);
	std::vector<int> get_surface_from_com(std::vector<point> coms);

	std::vector<tri> get_surface_elems_by_id(int id);
	std::vector<std::vector<tri>> get_surface_elems_by_id(std::vector<int> ids);

	size_t get_element_by_coordinate(point points, int dim);
	std::vector<size_t> get_elements_by_coordinate(std::vector<point> points, int dim);

	dimensions get_surface_dimensions(int id);
	std::vector <dimensions> get_surface_dimensions(std::vector<int> id);

	parameterized_surface_point parameterize_on_surface(point coord, int id);
	std::vector<parameterized_surface_point> parameterize_on_surface(std::vector <point> coord, int id);
}


