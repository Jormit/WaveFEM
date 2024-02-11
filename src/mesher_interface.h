#pragma once
#include <vector>
#include "geometry.h"
#include <gmsh/gmsh.h_cwrap>

namespace mesher_interface {
	void initialize();

	// Supports STEP file
	int import_model(const std::string& filename);

	void mesh_model(int mesh_size_min, int mesh_size_max);

	void view_model();

	box get_bounding_box();
	box get_bounding_box(int dim, int tag);
	std::vector<box> get_bounding_box(int dim, std::vector<int> tags);

	int add_box(box b);

	std::vector<int> subtract(int id1, int id2, bool remove_tool = true);

	// Vector is indexed by node number - 1 !!!!!
	std::vector<node> get_all_nodes();
	std::vector<size_t> get_node_ids_in_volume(int id);
	void label_boundary_nodes(std::vector<node>& nodes, std::vector <int> surface_ids);

	tet assemble_tet(size_t n1, size_t n2, size_t n3, size_t n4);
	std::vector<tet> get_volume_elems(int id);
	std::vector<std::vector<tet>> get_volume_elems(std::vector<int> id);

	int get_surface_from_com(point_3d);
	std::vector<int> get_surface_from_com(std::vector<point_3d> coms);

	tri assemble_tri(size_t n1, size_t n2, size_t n3);
	std::vector<tri> get_surface_elems(int id);
	std::vector<std::vector<tri>> get_surface_elems(std::vector<int> ids);

	tri get_surface_element_by_coordinate(point_3d points);
	std::vector<tri> get_surface_element_by_coordinate(std::vector<point_3d> points);
	tri get_surface_element_by_parametric_coordinate(point_2d point, int id);

	rectangle get_surface_bounds(int id);
	std::vector <rectangle> get_surface_bounds(std::vector<int> id);

	point_2d parameterize_on_surface(point_3d coord, int id);
	std::vector<point_2d> parameterize_on_surface(std::vector <point_3d> coord, int id);

	void parameterize_surface_nodes(std::vector<node>& nodes, int surface_id, const std::vector<tri> elements);
	void parameterize_surface_nodes(std::vector<node>& nodes, std::vector <int> surface_id, const std::vector<std::vector<tri>> elements);

	std::optional<tet> get_volume_element_by_coordinate(point_3d points);

	std::vector <int> get_boundary_surfaces();
}


