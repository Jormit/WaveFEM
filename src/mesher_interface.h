#pragma once
#include <vector>
#include "geometry.h"
#include <gmsh/gmsh.h_cwrap>

namespace mesher_interface {
	void initialize();

	// Loads STEP file into GMSH environment.
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

	tet assemble_tet(size_t n1, size_t n2, size_t n3, size_t n4);
	std::vector<tet> get_all_volume_elems();

	int get_surface_from_com(point_3d);
	std::vector<int> get_surface_from_com(std::vector<point_3d> coms);

	tri assemble_tri(size_t n1, size_t n2, size_t n3);
	std::vector<tri> get_surface_elems_by_id(int id);
	std::vector<std::vector<tri>> get_surface_elems_by_id(std::vector<int> ids);

	size_t get_element_by_coordinate(point_3d points, int dim);
	std::vector<size_t> get_elements_by_coordinate(std::vector<point_3d> points, int dim);

	rectangle get_surface_bounds(int id);
	std::vector <rectangle> get_surface_bounds(std::vector<int> id);

	point_2d parameterize_on_surface(point_3d coord, int id);
	std::vector<point_2d> parameterize_on_surface(std::vector <point_3d> coord, int id);

	void parameterize_surface_nodes(std::vector<node>& nodes, int surface_id, const std::vector<tri> elements);
	void parameterize_surface_nodes(std::vector<node>& nodes, std::vector <int> surface_id, const std::vector<std::vector<tri>> elements);
}


