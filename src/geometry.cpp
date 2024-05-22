#include <cmath>

#include "geometry.h"

Eigen::Vector2d point_2d::to_Eigen() const
{
	return Eigen::Vector2d(u, v);
}

Eigen::Vector3d point_3d::to_Eigen() const
{
	return Eigen::Vector3d(x, y, z);
}

void rectangle::add_padding(double x, double y)
{
	xmin -= x;
	ymin -= y;
	xmax += x;
	ymax += y;
}

void rectangle::add_padding(point_2d padding)
{
	add_padding(padding.u, padding.v);
}

void box::add_padding(double x, double y, double z)
{
	xmin -= x;
	ymin -= y;
	zmin -= z;
	xmax += x;
	ymax += y;
	zmax += z;
}

void box::add_padding(point_3d padding)
{
	add_padding(padding.x, padding.y, padding.z);
}

void box::add_padding(double padding)
{
	add_padding(padding, padding, padding);
}

double box::x_dim()
{
	return std::abs(xmax - xmin);
}

double box::y_dim()
{
	return std::abs(ymax - ymin);
}

double box::z_dim()
{
	return std::abs(zmax - zmin);
}

std::array<size_t, 2> tri::get_edge_nodes(size_t edge) const
{
	switch (edge) {
	case 0: return { nodes[0], nodes[1] };
	case 1: return { nodes[0], nodes[2] };
	case 2: return { nodes[1], nodes[2] };
	}
	return { 0, 0 };
}

Eigen::Matrix<double, 3, 2> tri::coordinate_matrix(const std::vector<node>& nodes) const
{
	Eigen::Matrix<double, 3, 2> coords;
	coords <<
		nodes[this->nodes[0] - 1].point_2d->u, nodes[this->nodes[0] - 1].point_2d->v,
		nodes[this->nodes[1] - 1].point_2d->u, nodes[this->nodes[1] - 1].point_2d->v,
		nodes[this->nodes[2] - 1].point_2d->u, nodes[this->nodes[2] - 1].point_2d->v;
	return coords;
}

Eigen::Matrix<double, 3, 2> tri::coordinate_matrix(const std::vector<node>& nodes, slice_plane plane) const
{
	Eigen::Matrix<double, 3, 2> coords;

	if (plane == slice_plane::XY)
	{
		coords <<
			nodes[this->nodes[0] - 1].coords.x, nodes[this->nodes[0] - 1].coords.y,
			nodes[this->nodes[1] - 1].coords.x, nodes[this->nodes[1] - 1].coords.y,
			nodes[this->nodes[2] - 1].coords.x, nodes[this->nodes[2] - 1].coords.y;
	} 
	else if (plane == slice_plane::XZ)
	{
		coords <<
			nodes[this->nodes[0] - 1].coords.x, nodes[this->nodes[0] - 1].coords.z,
			nodes[this->nodes[1] - 1].coords.x, nodes[this->nodes[1] - 1].coords.z,
			nodes[this->nodes[2] - 1].coords.x, nodes[this->nodes[2] - 1].coords.z;
	}
	else
	{
		coords <<
			nodes[this->nodes[0] - 1].coords.y, nodes[this->nodes[0] - 1].coords.z,
			nodes[this->nodes[1] - 1].coords.y, nodes[this->nodes[1] - 1].coords.z,
			nodes[this->nodes[2] - 1].coords.y, nodes[this->nodes[2] - 1].coords.z;
	}	
	
	return coords;
}

Eigen::Vector3d tri::lambda_to_coord(const std::vector<node>& nodes, Eigen::Vector3d lambda) const
{
	Eigen::Matrix<double, 3, 3> coords;

	coords <<
		nodes[this->nodes[0] - 1].coords.x, nodes[this->nodes[0] - 1].coords.y, nodes[this->nodes[0] - 1].coords.z,
		nodes[this->nodes[1] - 1].coords.x, nodes[this->nodes[1] - 1].coords.y, nodes[this->nodes[1] - 1].coords.z,
		nodes[this->nodes[2] - 1].coords.x, nodes[this->nodes[2] - 1].coords.y, nodes[this->nodes[2] - 1].coords.z;

	return coords * lambda;
}

std::vector<point_3d> generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z)
{
	std::vector<point_3d> points;
	points.reserve(num_x * num_y * num_z);
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			for (int z = 0; z < num_z; z++)
			{
				double point_x = static_cast<double>(x) / static_cast<double>(num_x - 1) * (box.xmax - box.xmin) + box.xmin;
				double point_y = static_cast<double>(y) / static_cast<double>(num_y - 1) * (box.ymax - box.ymin) + box.ymin;
				double point_z = static_cast<double>(z) / static_cast<double>(num_z - 1) * (box.zmax - box.zmin) + box.zmin;
				points.push_back({ point_x, point_y, point_z });
			}
		}
	}
	return points;
}

std::vector<point_2d> generate_grid_points(rectangle rect, size_t num_x, size_t num_y)
{
	std::vector<point_2d> points;
	points.reserve(num_x * num_y);
	for (int x = 0; x < num_x; x++)
	{
		for (int y = 0; y < num_y; y++)
		{
			double point_x = static_cast<double>(x) / static_cast<double>(num_x - 1) * (rect.xmax - rect.xmin) + rect.xmin;
			double point_y = static_cast<double>(y) / static_cast<double>(num_y - 1) * (rect.ymax - rect.ymin) + rect.ymin;
			points.push_back({ point_x, point_y });
		}
	}
	return points;
}

std::array<size_t, 2> tet::get_edge_nodes(size_t edge) const
{
	switch (edge)
	{
	case 0: return { nodes[0], nodes[1] };
	case 1: return { nodes[0], nodes[2] };
	case 2: return { nodes[0], nodes[3] };
	case 3: return { nodes[1], nodes[2] };
	case 4: return { nodes[1], nodes[3] };
	case 5: return { nodes[2], nodes[3] };
	}
	return { 0, 0 };
}

std::array<size_t, 3> tet::get_face_nodes(size_t face) const
{
	switch (face) {
	case 0: return { nodes[0], nodes[1], nodes[2] };
	case 1: return { nodes[0], nodes[1], nodes[3] };
	case 2: return { nodes[0], nodes[2], nodes[3] };
	case 3: return { nodes[1], nodes[2], nodes[3] };
	}
	return { 0, 0, 0 };
}

Eigen::Matrix<double, 4, 3> tet::coordinate_matrix(const std::vector<node>& nodes) const
{
	Eigen::Matrix<double, 4, 3> coords;
	coords <<
		nodes[this->nodes[0] - 1].coords.x, nodes[this->nodes[0] - 1].coords.y, nodes[this->nodes[0] - 1].coords.z,
		nodes[this->nodes[1] - 1].coords.x, nodes[this->nodes[1] - 1].coords.y, nodes[this->nodes[1] - 1].coords.z,
		nodes[this->nodes[2] - 1].coords.x, nodes[this->nodes[2] - 1].coords.y, nodes[this->nodes[2] - 1].coords.z,
		nodes[this->nodes[3] - 1].coords.x, nodes[this->nodes[3] - 1].coords.y, nodes[this->nodes[3] - 1].coords.z;
	return coords;
}

point_3d tet::get_center(const std::vector<node>& nodes) const
{
	auto x_center = nodes[this->nodes[0] - 1].coords.x + nodes[this->nodes[1] - 1].coords.x + nodes[this->nodes[2] - 1].coords.x + nodes[this->nodes[3] - 1].coords.x;
	auto y_center = nodes[this->nodes[0] - 1].coords.y + nodes[this->nodes[1] - 1].coords.y + nodes[this->nodes[2] - 1].coords.y + nodes[this->nodes[3] - 1].coords.y;
	auto z_center = nodes[this->nodes[0] - 1].coords.z + nodes[this->nodes[1] - 1].coords.z + nodes[this->nodes[2] - 1].coords.z + nodes[this->nodes[3] - 1].coords.z;

	return { x_center/3, y_center/3, z_center/3 };
}

structured_grid_2d::structured_grid_2d(rectangle rect, size_t num_x, size_t num_y)
{
	this->start_point = { rect.xmin, rect.ymin };
	this->num_steps = { num_x, num_y };
	this->step_sizes = {
		1 / static_cast<double>(num_x - 1) * (rect.xmax - rect.xmin),
		1 / static_cast<double>(num_y - 1) * (rect.ymax - rect.ymin)
	};	
}

structured_grid_3d::structured_grid_3d(box box, size_t num_x, size_t num_y, size_t num_z)
{
	this->start_point = { box.xmin, box.ymin, box.zmin };
	this->num_steps = { num_x, num_y, num_z };
	this->step_sizes = {
		1 / static_cast<double>(num_x - 1) * (box.xmax - box.xmin),
		1 / static_cast<double>(num_y - 1) * (box.ymax - box.ymin),
		1 / static_cast<double>(num_z - 1) * (box.zmax - box.zmin)
	};
}

Eigen::Vector3cd normal_cross_on_box_face(size_t face_num, Eigen::Vector2cd surface_vec)
{
	Eigen::Vector3cd result;
	if (face_num == 0 || face_num == 1)
	{
		result << surface_vec(1), -surface_vec(0), 0;
	}
	else if (face_num == 2 || face_num == 3)
	{
		result << surface_vec(2), 0, -surface_vec(0);
	}
	else
	{
		result << 0, surface_vec(2), -surface_vec(1);
	}
	return result;
}