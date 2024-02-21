#include <format>
#include <fstream>
#include <iostream>

#include "post.h"
#include "mesher_interface.h"
#include "fem.h"

std::vector<std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd>> post::eval_port(const sim& sim_instance, size_t num_x, size_t num_y)
{
	std::vector<std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd>> results;
	for (size_t p = 0; p < sim_instance.sim_ports.entity_ids.size(); p++)
	{
		for (size_t m = 0; m < sim_instance.port_eigen_vectors[0].cols(); m++)
		{
			results.push_back(eval_port(sim_instance, p, m, num_x, num_y));
		}
	}
	return results;
}

std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd> post::eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	Eigen::MatrixX2d point (points.size(), 2);
	Eigen::MatrixX2cd field (points.size(), 2);

	for (int i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance.sim_ports.entity_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance.nodes,
			e, {p.u, p.v}, sim_instance.port_dof_maps[port_num], sim_instance.port_eigen_vectors[port_num].col(mode));
		point.row(i) << p.u, p.v;
		field.row(i) << elem_field(0), elem_field(1);
	}

	return { point, field };
}

std::vector<std::pair<Eigen::MatrixX3d, Eigen::MatrixX3cd>> post::eval_full(const sim& sim_instance, size_t num_x, size_t num_y, size_t num_z)
{
	std::vector<std::pair<Eigen::MatrixX3d, Eigen::MatrixX3cd>> results;
	for (size_t p = 0; p < sim_instance.sim_ports.entity_ids.size(); p++)
	{
		results.push_back(eval_full(sim_instance, p, num_x, num_y, num_z));
	}
	return results;
}

std::pair<Eigen::MatrixX3d, Eigen::MatrixX3cd> post::eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z)
{
	auto points = generate_grid_points(sim_instance.bbox, num_x, num_y, num_z);

	Eigen::MatrixX3d point(points.size(), 3);
	Eigen::MatrixX3cd field(points.size(), 3);

	for (int i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_volume_element_by_coordinate(p);

		if (!e.has_value())
		{
			point.row(i) << p.x, p.y, p.z;
			field.row(i) << 0, 0, 0;
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance.nodes,
			e.value(), p, sim_instance.full_dof_map, sim_instance.full_solutions[port_num]);

		point.row(i) << p.x, p.y, p.z;
		field.row(i) << elem_field(0), elem_field(1), elem_field(2);
	}

	return { point, field };
}
