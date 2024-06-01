#include <cmath>

#include "geometry.h"

Eigen::Vector2d geo::point_2d::to_Eigen() const
{
	return Eigen::Vector2d(u, v);
}

Eigen::Vector3d geo::point_3d::to_Eigen() const
{
	return Eigen::Vector3d(x, y, z);
}

void geo::rectangle::add_padding(double x, double y)
{
	xmin -= x;
	ymin -= y;
	xmax += x;
	ymax += y;
}

void geo::rectangle::add_padding(point_2d padding)
{
	add_padding(padding.u, padding.v);
}

void geo::box::add_padding(double x, double y, double z)
{
	xmin -= x;
	ymin -= y;
	zmin -= z;
	xmax += x;
	ymax += y;
	zmax += z;
}

void geo::box::add_padding(point_3d padding)
{
	add_padding(padding.x, padding.y, padding.z);
}

void geo::box::add_padding(double padding)
{
	add_padding(padding, padding, padding);
}

double geo::box::x_dim()
{
	return std::abs(xmax - xmin);
}

double geo::box::y_dim()
{
	return std::abs(ymax - ymin);
}

double geo::box::z_dim()
{
	return std::abs(zmax - zmin);
}

std::array<size_t, 2> geo::tri::get_edge_nodes(size_t edge) const
{
	switch (edge) {
	case 0: return { nodes[0], nodes[1] };
	case 1: return { nodes[0], nodes[2] };
	case 2: return { nodes[1], nodes[2] };
	}
	return { 0, 0 };
}

Eigen::Matrix<double, 3, 2> geo::tri::coordinate_matrix(const std::vector<node>& nodes) const
{
	Eigen::Matrix<double, 3, 2> coords;
	coords <<
		nodes[this->nodes[0] - 1].point_2d->u, nodes[this->nodes[0] - 1].point_2d->v,
		nodes[this->nodes[1] - 1].point_2d->u, nodes[this->nodes[1] - 1].point_2d->v,
		nodes[this->nodes[2] - 1].point_2d->u, nodes[this->nodes[2] - 1].point_2d->v;
	return coords;
}

std::array<size_t, 2> geo::tet::get_edge_nodes(size_t edge) const
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

std::array<size_t, 3> geo::tet::get_face_nodes(size_t face) const
{
	switch (face) {
	case 0: return { nodes[0], nodes[1], nodes[2] };
	case 1: return { nodes[0], nodes[1], nodes[3] };
	case 2: return { nodes[0], nodes[2], nodes[3] };
	case 3: return { nodes[1], nodes[2], nodes[3] };
	}
	return { 0, 0, 0 };
}

Eigen::Matrix<double, 4, 3> geo::tet::coordinate_matrix(const std::vector<node>& nodes) const
{
	Eigen::Matrix<double, 4, 3> coords;
	coords <<
		nodes[this->nodes[0] - 1].coords.x, nodes[this->nodes[0] - 1].coords.y, nodes[this->nodes[0] - 1].coords.z,
		nodes[this->nodes[1] - 1].coords.x, nodes[this->nodes[1] - 1].coords.y, nodes[this->nodes[1] - 1].coords.z,
		nodes[this->nodes[2] - 1].coords.x, nodes[this->nodes[2] - 1].coords.y, nodes[this->nodes[2] - 1].coords.z,
		nodes[this->nodes[3] - 1].coords.x, nodes[this->nodes[3] - 1].coords.y, nodes[this->nodes[3] - 1].coords.z;
	return coords;
}

geo::point_3d geo::tet::get_center(const std::vector<node>& nodes) const
{
	auto x_center = nodes[this->nodes[0] - 1].coords.x + nodes[this->nodes[1] - 1].coords.x + nodes[this->nodes[2] - 1].coords.x + nodes[this->nodes[3] - 1].coords.x;
	auto y_center = nodes[this->nodes[0] - 1].coords.y + nodes[this->nodes[1] - 1].coords.y + nodes[this->nodes[2] - 1].coords.y + nodes[this->nodes[3] - 1].coords.y;
	auto z_center = nodes[this->nodes[0] - 1].coords.z + nodes[this->nodes[1] - 1].coords.z + nodes[this->nodes[2] - 1].coords.z + nodes[this->nodes[3] - 1].coords.z;

	return { x_center/3, y_center/3, z_center/3 };
}

geo::structured_grid_2d::structured_grid_2d(rectangle rect, size_t num_x, size_t num_y)
{
	this->start_point = { rect.xmin, rect.ymin };
	this->num_steps = { num_x, num_y };
	this->step_sizes = {
		1 / static_cast<double>(num_x - 1) * (rect.xmax - rect.xmin),
		1 / static_cast<double>(num_y - 1) * (rect.ymax - rect.ymin)
	};	
}

geo::structured_grid_3d::structured_grid_3d(box box, size_t num_x, size_t num_y, size_t num_z)
{
	this->start_point = { box.xmin, box.ymin, box.zmin };
	this->num_steps = { num_x, num_y, num_z };
	this->step_sizes = {
		1 / static_cast<double>(num_x - 1) * (box.xmax - box.xmin),
		1 / static_cast<double>(num_y - 1) * (box.ymax - box.ymin),
		1 / static_cast<double>(num_z - 1) * (box.zmax - box.zmin)
	};
}

std::vector<geo::point_3d> geo::generate_grid_points(box box, size_t num_x, size_t num_y, size_t num_z)
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

std::vector<geo::point_2d> geo::generate_grid_points(rectangle rect, size_t num_x, size_t num_y)
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

std::vector<geo::point_2d> geo::generate_grid_points(structured_grid_2d grid)
{
	std::vector<point_2d> points;
	points.reserve(grid.num_steps[0] * grid.num_steps[1]);
	for (size_t i = 0; i < grid.num_steps[0]; i++)
	{
		for (size_t j = 0; j < grid.num_steps[1]; j++)
		{
			points.push_back({
				grid.start_point[0] + i * grid.step_sizes[0],
				grid.start_point[1] + j * grid.step_sizes[1] });
		}
	}
	return points;
}

std::vector<geo::point_3d> geo::generate_grid_points(structured_grid_3d grid)
{
	std::vector<point_3d> points;
	points.reserve(grid.num_steps[0] * grid.num_steps[1] * grid.num_steps[2]);
	for (size_t i = 0; i < grid.num_steps[0]; i++)
	{
		for (size_t j = 0; j < grid.num_steps[1]; j++)
		{
			for (size_t k = 0; k < grid.num_steps[2]; k++)
			{
				points.push_back({
					grid.start_point[0] + i * grid.step_sizes[0],
					grid.start_point[1] + j * grid.step_sizes[1],
					grid.start_point[2] + k * grid.step_sizes[2]});
			}
		}
	}
	return points;
}

std::map<size_t, std::pair<size_t, size_t>> geo::generate_face_to_element_map(const std::vector<geo::tet>& elems)
{
	std::map<size_t, std::pair<size_t, size_t>> map;
	for (size_t i = 0; i < elems.size(); i++)
	{
		auto e = elems[i];
		for (auto f : e.faces)
		{
			if (map.contains(f))
			{
				map[f].second = i;
			}
			else
			{
				map[f] = { i, i };
			}
		}
	}

	return map;
}
