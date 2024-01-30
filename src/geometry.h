#pragma once
#include <ostream>
#include <array>
#include <vector>
#include <optional>
#include <list>
#include <Eigen/dense>

const int BOUNDARY_NODE = 0;
const int FREE_NODE = -1;

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

class box
{
public:
	double xmin, ymin, zmin, xmax, ymax, zmax;
	void add_padding(double x, double y, double z);
	void add_padding(point_3d padding);
};

struct node
{
	point_3d coords;
	std::optional<point_2d> point_2d;
	int type_2d;
	int type_3d;
	std::list<size_t> surface_entities;
};

struct tri
{
	std::array<size_t, 3> nodes;
	std::array<size_t, 3> edges;
	size_t face;
	size_t material_id;
	std::array<size_t, 2> get_edge_nodes(size_t edge) const;
	Eigen::Matrix<double, 3, 2> coordinate_matrix(const std::vector<node>& nodes) const;
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
};

std::vector<point_3d> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z);
std::vector<point_2d> generate_grid_points(rectangle rect, size_t num_x, size_t num_y);