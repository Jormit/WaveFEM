#pragma once
#include <ostream>
#include <array>
#include <vector>
#include <optional>
#include <Eigen/dense>
#include <map>

namespace geo {
	const int BOUNDARY = 0;
	const int PORT_OUTER_BOUNDARY = -1;

	enum box_face {
		XY_BOTTOM = 0,
		XY_TOP = 1,
		XZ_BOTTOM = 2,
		XZ_TOP = 3,
		YZ_BOTTOM = 4,
		YZ_TOP = 5
	};

	enum far_field_slice { THETA, PHI };

	struct point_2d
	{
		double u;
		double v;
		Eigen::Vector2d to_Eigen() const;
	};

	struct point_3d
	{
		double x;
		double y;
		double z;
		Eigen::Vector3d to_Eigen() const;
	};

	struct rectangle
	{
		double xmin, ymin, xmax, ymax;
		void add_padding(double x, double y);
		void add_padding(point_2d padding);
	};

	struct box
	{
		double xmin, ymin, zmin, xmax, ymax, zmax;
		void add_padding(double x, double y, double z);
		void add_padding(point_3d padding);
		void add_padding(double padding);

		double x_dim();
		double y_dim();
		double z_dim();
	};

	struct node
	{
		point_3d coords;
		std::optional<point_2d> point_2d;
		bool boundary_2d;
	};

	struct tri
	{
		std::array<size_t, 3> nodes;
		std::array<size_t, 3> edges;
		size_t face;
		size_t material_id;
		std::array<size_t, 2> get_edge_nodes(size_t edge) const;
		Eigen::Matrix<double, 3, 2> coordinate_matrix(const std::vector<node>& nodes) const;
		Eigen::Vector3d lambda_to_coord(const std::vector<node>& nodes, Eigen::Vector3d lambda) const;
	};

	struct tet
	{
		std::array<size_t, 4> nodes;
		std::array<size_t, 6> edges;
		std::array<size_t, 4> faces;
		size_t material_id;
		std::array<size_t, 2> get_edge_nodes(size_t edge) const;
		std::array<size_t, 3> get_face_nodes(size_t face) const;
		Eigen::Matrix<double, 4, 3> coordinate_matrix(const std::vector<node>& nodes) const;
		point_3d get_center(const std::vector<node>& nodes) const;
	};

	struct structured_grid_2d
	{
		std::array <double, 2> start_point;
		std::array <double, 2> step_sizes;
		std::array <size_t, 2> num_steps;
		structured_grid_2d(rectangle rect, size_t num_x, size_t num_y);
	};

	struct structured_grid_3d
	{
		std::array <double, 3> start_point;
		std::array <double, 3> step_sizes;
		std::array <size_t, 3> num_steps;
		structured_grid_3d(box box, size_t num_x, size_t num_y, size_t num_z);
	};

	struct structured_2d_field_data
	{
		structured_grid_2d grid;
		Eigen::MatrixX2cd field;
	};

	struct structured_3d_field_data
	{
		structured_grid_3d grid;
		Eigen::MatrixX3cd field;
	};

	struct unstructured_3d_field_data
	{
		Eigen::MatrixX3d points;
		Eigen::MatrixX3cd field;
	};

	struct structured_polar_2d
	{
		double start_angle;
		double angle_step;
		size_t num_steps;

		far_field_slice type;
		double fixed_angle;
	};

	struct polar_2d_field_data
	{
		structured_polar_2d sweep;
		Eigen::MatrixX3cd field;
	};

	std::vector<point_3d> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z);
	std::vector<point_2d> generate_grid_points(rectangle rect, size_t num_x, size_t num_y);
	std::vector<point_2d> generate_grid_points(structured_grid_2d grid);
	std::vector<point_3d> generate_grid_points(structured_grid_3d grid);

	std::map<size_t, std::pair<size_t, size_t>> generate_face_to_element_map(const std::vector<geo::tet>& elems);

	Eigen::Vector3d box_face_normal(box_face id);
}