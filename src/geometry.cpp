#include "geometry.h"

box::box(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax)
	: xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax)
{}

void box::add_padding(double x, double y, double z)
{
	xmin -= x;
	ymin -= y;
	zmin -= z;
	xmax += x;
	ymax += y;
	zmax += z;
}

std::ostream& operator << (std::ostream& os, box const& b) 
{
	return os
		<< "xmin: " << b.xmin << " ymin: " << b.ymin
		<< " zmin: " << b.zmin << " xmax: " << b.xmax
		<< " ymax: " << b.ymax << " zmax: " << b.zmax;
}

node::node(std::array<double, 3> coords, int type_2d, int type_3d)
	: coords(coords), type_2d(type_2d), type_3d(type_3d)
{}

tri::tri(std::array<size_t, 3> nodes, std::array<size_t, 3> edges, size_t face, size_t tag)
	: nodes(nodes), edges(edges), face(face), tag(tag)
{}

std::array<size_t, 2> tri::get_edge_nodes(size_t edge) const 
{
	switch (edge) {
	case 0: return { nodes[0], nodes[1] };
	case 1: return { nodes[0], nodes[2] };
	case 2: return { nodes[1], nodes[2] };
	}
}

tet::tet(std::array<size_t, 4> nodes, std::array<size_t, 6> edges, std::array<size_t, 4> faces, size_t tag)
	: nodes(nodes), edges(edges), faces(faces), tag(tag)
{}

std::vector<std::array<double,3>> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z)
{
	std::vector<std::array<double, 3>> points;
	points.reserve(num_x * num_y * num_z);
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			for (int z = 0; z < num_z; z++)
			{
				double point_x = static_cast<double>(x) / static_cast<double>(num_x - 1) * (box.xmax - box.xmin);
				double point_y = static_cast<double>(y) / static_cast<double>(num_y - 1) * (box.ymax - box.ymin);
				double point_z = static_cast<double>(z) / static_cast<double>(num_z - 1) * (box.zmax - box.zmin);
				points.push_back({ point_x, point_y, point_z });
			}
		}
	}
	return points;
}