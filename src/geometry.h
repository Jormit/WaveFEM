#pragma once
#include <ostream>
#include <array>
#include <vector>

const int BOUNDARY_NODE = 0;
const int FREE_NODE = -1;

class box
{
public:
	double xmin, ymin, zmin, xmax, ymax, zmax;

	box(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax);
	void add_padding(double x, double y, double z);
	friend std::ostream& operator << (std::ostream& os, box const& b);
};

struct node
{
	std::array<double, 3> coords;
	int type_2d;
	int type_3d;
	node(std::array<double, 3> coords, int type_2d, int type_3d);
	node() = default;
};

struct tri
{
	std::array<size_t, 3> nodes;
	std::array<size_t, 3> edges;
	size_t face;
	size_t tag;

	tri(std::array<size_t, 3> nodes, std::array<size_t, 3> edges, size_t face, size_t tag);
	std::array<size_t, 2> get_edge_nodes(size_t edge) const;
};

struct tet
{
	std::array<size_t, 4> nodes;
	std::array<size_t, 6> edges;
	std::array<size_t, 4> faces;

	size_t tag;
	tet(std::array<size_t, 4> nodes, std::array<size_t, 6> edges, std::array<size_t, 4> faces, size_t tag);
};

std::vector<std::array<double, 3>> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z);