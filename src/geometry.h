#pragma once
#include <ostream>
#include <array>
#include <vector>
#include <optional>

const int BOUNDARY_NODE = 0;
const int FREE_NODE = -1;

struct rectangle
{
	double xmin, ymin, xmax, ymax;

	void add_padding(double x, double y);
};

class box
{
public:
	double xmin, ymin, zmin, xmax, ymax, zmax;

	void add_padding(double x, double y, double z);
	friend std::ostream& operator << (std::ostream& os, box const& b);
};

struct point_2d
{
	double u;
	double v;
};

struct point_3d
{
	double x;
	double y;
	double z;
};

struct node
{
	point_3d coords;
	std::optional<point_2d> point_2d;
	int type_2d;
	int type_3d;
};

struct tri
{
	std::array<size_t, 3> nodes;
	std::array<size_t, 3> edges;
	size_t face;
	size_t tag;
	std::array<size_t, 2> get_edge_nodes(size_t edge) const;
};

struct tet
{
	std::array<size_t, 4> nodes;
	std::array<size_t, 6> edges;
	std::array<size_t, 4> faces;
	size_t tag;
};

std::vector<point_3d> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z);
std::vector<point_2d> generate_grid_points(rectangle rect, size_t num_x, size_t num_y);