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
	std::array<size_t, 3> nodes;
	std::array<size_t, 3> edges;
	size_t face;
	size_t tag;
	tri(std::array<size_t, 3> nodes, std::array<size_t, 3> edges, size_t face, size_t tag)
		: nodes(nodes), edges(edges), face(face), tag(tag)
	{}

	std::array<size_t, 2> get_edge_nodes(size_t edge) const {
		switch (edge) {
		case 0: return { nodes[0], nodes[1] };
		case 1: return { nodes[0], nodes[2] };
		case 2: return { nodes[1], nodes[2] };
		}
	}
};

struct tet
{
	std::array<size_t, 4> nodes;
	std::array<size_t, 6> edges;
	std::array<size_t, 4> faces;
	size_t tag;
	tet(std::array<size_t, 4> nodes, std::array<size_t, 6> edges, std::array<size_t, 4> faces, size_t tag)
		: nodes(nodes), edges(edges), faces(faces), tag(tag)
	{}
};