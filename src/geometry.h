#pragma once
#include <ostream>
#include <array>
#include <vector>
#include <optional>

const int BOUNDARY_NODE = 0;
const int FREE_NODE = -1;

class box
{
public:
	double xmin, ymin, zmin, xmax, ymax, zmax;

	void add_padding(double x, double y, double z);
	friend std::ostream& operator << (std::ostream& os, box const& b);
};

struct parameterized_surface_point
{
	double u;
	double v;
};

struct dimensions
{
	double width;
	double height;
};

struct point
{
	double x;
	double y;
	double z;
};

struct node
{
	point coords;
	std::optional<parameterized_surface_point> parameterized_surface_point;
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

std::vector<point> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z);