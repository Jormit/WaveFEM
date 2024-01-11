#pragma once
#include <ostream>
#include <array>

const int BOUNDARY_NODE = 0;
const int FREE_NODE = -1;

class box
{
public:
	double xmin, ymin, zmin, xmax, ymax, zmax;

	box(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax) 
		: xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax) 
	{}

	void add_padding(double x, double y, double z)
	{
		xmin -= x;
		ymin -= y;
		zmin -= z;
		xmax += x;
		ymax += y;
		zmax += z;
	}

	friend std::ostream& operator << (std::ostream& os, box const& b) {
		return os 
			<< "xmin: " << b.xmin << " ymin: " << b.ymin 
			<< " zmin: " << b.zmin << " xmax: " << b.xmax 
			<< " ymax: " << b.ymax << " zmax: " << b.zmax;
	}
};

struct node
{
	std::array<double, 3> coords;
	int tag;
	node(std::array<double, 3> coords, int tag)
		: coords(coords), tag(tag)
	{}

	node() = default;
};

struct tri
{
	std::array<int, 3> nodes;
	int tag;
	tri(std::array<int, 3> nodes, int tag)
		: nodes(nodes), tag(tag)
	{}

	std::array<int, 2> get_edge_nodes(int edge) {
		switch (edge) {
		case 0: return { nodes[0], nodes[1] };
		case 1: return { nodes[0], nodes[2] };
		case 2: return { nodes[1], nodes[2] };
		default: return { nodes[0], nodes[0] };
		}
	}
};

struct tet
{
	std::array<int, 4> nodes;
	int tag;
	tet(std::array<int, 4> nodes, int tag)
		: nodes(nodes), tag(tag)
	{}

	std::array<int, 2> get_edge_nodes(int edge) {
		switch (edge) {
		case 0: return { nodes[0], nodes[1] };
		case 1: return { nodes[0], nodes[2] };
		case 2: return { nodes[0], nodes[3] };
		case 3: return { nodes[1], nodes[2] };
		case 4: return { nodes[1], nodes[3] };
		case 5: return { nodes[2], nodes[3] };
		default: return { nodes[0], nodes[0] };
		}
	}

	std::array<int, 3> get_face_nodes(int face) {
		switch (face) {
		case 0: return { nodes[0], nodes[1], nodes[2] };
		case 1: return { nodes[0], nodes[1], nodes[3] };
		case 2: return { nodes[0], nodes[2], nodes[3] };
		case 3: return { nodes[1], nodes[2], nodes[3] };
		default: return { nodes[0], nodes[0], nodes[0] };
		}
	}
};