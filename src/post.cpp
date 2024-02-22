#include <iostream>
#include <cmath>

#include "post.h"
#include "mesher_interface.h"
#include "fem.h"
#include "helpers.h"

std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd> post::eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	Eigen::MatrixX2d point (points.size(), 2);
	Eigen::MatrixX2cd field (points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance.sim_ports.entity_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance.nodes,
				e, { p.u, p.v }, sim_instance.port_dof_maps[port_num], sim_instance.port_eigen_vectors[port_num].col(mode));
		
		point.row(i) << p.u, p.v;
		field.row(i) << elem_field(0), elem_field(1);
	}

	return { point, field };
}

std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd> post::eval_port_from_3d(const sim& sim_instance, size_t eval_port_num, size_t driven_port_num, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.bounds[eval_port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	Eigen::MatrixX2d point(points.size(), 2);
	Eigen::MatrixX2cd field(points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance.sim_ports.entity_ids[eval_port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance.nodes,
			e, { p.u, p.v }, sim_instance.full_dof_map, sim_instance.full_solutions[driven_port_num]);

		point.row(i) << p.u, p.v;
		field.row(i) << elem_field(0), elem_field(1);
	}

	return { point, field };
}

std::pair<Eigen::MatrixX3d, Eigen::MatrixX3cd> post::eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z)
{
	auto points = generate_grid_points(sim_instance.bbox, num_x, num_y, num_z);

	Eigen::MatrixX3d point(points.size(), 3);
	Eigen::MatrixX3cd field(points.size(), 3);

	for (size_t i = 0; i < points.size(); i++)
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

Eigen::MatrixXcd post::eval_s_parameters(const sim& sim_instance, size_t num_x, size_t num_y)
{
	size_t num_ports = sim_instance.sim_ports.entity_ids.size();
	Eigen::MatrixXcd s_params (num_ports, num_ports);
	
	for (size_t j = 0; j < num_ports; j++)
	{
		for (size_t i = 0; i < num_ports; i++)
		{			
			auto port_i_3d = post::eval_port_from_3d(sim_instance, i, j, num_x, num_y);
			auto i_field_vec_3d = port_i_3d.second;

			auto port_i_2d = post::eval_port(sim_instance, i, 0, num_x, num_y);
			auto i_field_vec_2d = port_i_2d.second;
			auto i_scaling = std::sqrt(helpers::rowise_2d_dot_product(i_field_vec_2d, i_field_vec_2d));

			auto port_j = post::eval_port(sim_instance, j, 0, num_x, num_y);
			auto j_field_vec = port_j.second;
			auto j_scaling = std::sqrt(helpers::rowise_2d_dot_product(j_field_vec, j_field_vec));

			if (i == j)
			{
				s_params(i, j) = helpers::rowise_2d_dot_product(i_field_vec_3d - j_field_vec, j_field_vec) / (j_scaling * j_scaling);
			} 
			else
			{
				s_params(i, j) = helpers::rowise_2d_dot_product(i_field_vec_3d, i_field_vec_2d) / (j_scaling * i_scaling);
			}
		}		
	}
	return s_params;
}