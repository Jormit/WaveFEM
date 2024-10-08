#pragma once
#include <vector>
#include <unordered_set>
#include <utility>
#include <string>
#include <array>
#include <Eigen/dense>

#include "geometry.h"
#include <gmsh/gmsh.h_cwrap>

namespace mesher_interface {
	void initialize();

	// Supports STEP file
	std::vector<int> import_step(const std::string& filename);

	void mesh_model(double mesh_size_min, double mesh_size_max);

	void view_model();

	geo::box get_bounding_box();
	geo::box get_bounding_box(int dim, int tag);
	std::vector<geo::box> get_bounding_box(int dim, std::vector<int> tags);

	std::vector<int> get_entities_in_bounding_box(int dim, geo::box box);
	std::vector<std::vector<int>> get_entities_in_bounding_box(int dim, std::vector<geo::box> box);

	int get_surface_parent(int id);
	std::vector<int> get_surface_parent(std::vector<int> id);

	int add_box(geo::box b);

	std::vector<int> subtract(int obj, int tool, bool remove_tool = true);
	std::vector<int> subtract(std::vector<int> obj, int tool, bool remove_tool = true);

	int fuse_surfaces(std::vector<int> obj, bool remove_obj);
	std::vector<int> fuse_surfaces(std::vector <std::vector<int>> obj, bool remove_obj);

	void remove_duplicates();

	geo::point_3d evaluate_surface_parameterization(int surface_id, geo::point_2d point);

	Eigen::Matrix<double, 3, 2> evaluate_surface_derivative(int surface_id, geo::point_2d point);

	std::vector<geo::node> get_all_nodes();
	std::vector<size_t> get_node_ids_in_line(int id);
	std::vector<size_t> get_node_ids_in_volume(int id);

	std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> get_surface_edges_and_faces(int surface_id);
	std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> get_surface_edges_and_faces(std::vector <int> surface_ids);

	std::vector<int> get_surface_boundary_entities(int surface_id);
	std::vector<int> get_surface_boundary_entities(std::vector <int> surface_ids);

	std::unordered_set<size_t> get_edges_on_line(int line_id);
	std::unordered_set<size_t> get_edges_on_line(std::vector <int> line_id);

	geo::tet assemble_tet(size_t n1, size_t n2, size_t n3, size_t n4);
	
	std::vector<geo::tet> get_all_volume_elems();
	std::vector<size_t> get_volume_elem_ids(int id);
	std::vector<std::vector<size_t>> get_volume_elem_ids(std::vector<int> id);

	void label_elems_in_volume(int id, size_t label, std::vector<geo::tet>& elems);
	void label_elems_in_volume(std::vector<int> ids, size_t label, std::vector<geo::tet>& elems);

	geo::tri assemble_tri(size_t n1, size_t n2, size_t n3);
	std::vector<geo::tri> get_surface_elems(int id);
	std::vector<std::vector<geo::tri>> get_surface_elems(std::vector<int> ids);

	geo::tri get_surface_element_by_coordinate(geo::point_3d points);
	std::vector<geo::tri> get_surface_element_by_coordinate(std::vector<geo::point_3d> points);
	geo::tri get_surface_element_by_parametric_coordinate(geo::point_2d point, int id);

	geo::rectangle get_surface_parametric_bounds(int id);
	std::vector <geo::rectangle> get_surface_parametric_bounds(std::vector<int> id);

	geo::point_2d parameterize_on_surface(geo::point_3d coord, int id);
	std::vector<geo::point_2d> parameterize_on_surface(std::vector <geo::point_3d> coord, int id);

	void parameterize_surface_nodes(std::vector<geo::node>& nodes, int surface_id, const std::vector<geo::tri> elements);
	void parameterize_surface_nodes(std::vector<geo::node>& nodes, std::vector <int> surface_id, const std::vector<std::vector<geo::tri>> elements);

	std::optional<size_t> get_volume_element_by_coordinate(geo::point_3d point);

	std::optional <size_t> get_volume_entity_by_coordinate(geo::point_3d point);
	
	std::vector <int> get_boundary_surfaces();

	std::vector<int> get_bounding_box_surfaces(geo::box box);

	void write_vtk(std::string filename);
}


