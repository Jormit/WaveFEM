#include <iostream>
#include <cmath>

#include "post.h"
#include "mesher_interface.h"
#include "fem.h"
#include "helpers.h"

structured_2d_field_data post::eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.parametric_bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	structured_grid_2d grid(bounds, num_x, num_y);
	Eigen::MatrixX2cd field (points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance.sim_ports.dummy_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance.nodes,
				e, { p.u, p.v }, sim_instance.port_dof_maps[port_num], sim_instance.port_eigen_vectors[port_num].col(mode));
		
		field.row(i) << elem_field(0), elem_field(1);
	}

	return { grid, field };
}

structured_2d_field_data post::eval_port_from_3d(const sim& sim_instance, size_t eval_port_num, size_t driven_port_num, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.parametric_bounds[eval_port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	structured_grid_2d grid(bounds, num_x, num_y);
	Eigen::MatrixX2cd field(points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance.sim_ports.dummy_ids[eval_port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance.nodes,
			e, { p.u, p.v }, sim_instance.full_dof_map, sim_instance.full_solutions[driven_port_num]);

		field.row(i) << elem_field(0), elem_field(1);
	}

	return { grid, field };
}

structured_3d_field_data post::eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z)
{
	auto points = generate_grid_points(sim_instance.bbox, num_x, num_y, num_z);

	structured_grid_3d grid(sim_instance.bbox, num_x, num_y, num_z);
	Eigen::MatrixX3cd field(points.size(), 3);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];
		auto e = mesher_interface::get_volume_element_by_coordinate(p);

		if (!e.has_value())
		{
			field.row(i) << NAN, NAN, NAN;
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance.nodes,
			e.value(), p, sim_instance.full_dof_map, sim_instance.full_solutions[port_num]);

		field.row(i) << elem_field(0), elem_field(1), elem_field(2);
	}

	return { grid, field };
}

Eigen::MatrixXcd post::eval_s_parameters(const sim& sim_instance, size_t num_x, size_t num_y)
{
	size_t num_ports = sim_instance.sim_ports.dummy_ids.size();
	Eigen::MatrixXcd s_params (num_ports, num_ports);
	
	for (size_t j = 0; j < num_ports; j++)
	{
		for (size_t i = 0; i < num_ports; i++)
		{			
			auto port_i_3d = post::eval_port_from_3d(sim_instance, i, j, num_x, num_y);

			auto port_i_2d = post::eval_port(sim_instance, i, 0, num_x, num_y);
			auto i_scaling = std::sqrt(helpers::rowise_2d_dot_product(port_i_2d.field, port_i_2d.field));

			auto port_j = post::eval_port(sim_instance, j, 0, num_x, num_y);
			auto j_scaling = std::sqrt(helpers::rowise_2d_dot_product(port_j.field, port_j.field));

			if (i == j)
			{
				s_params(i, j) = helpers::rowise_2d_dot_product((port_i_3d.field - port_j.field).eval(), port_j.field) / (j_scaling * j_scaling);
			} 
			else
			{
				s_params(i, j) = helpers::rowise_2d_dot_product(port_i_3d.field, port_i_2d.field) / (j_scaling * i_scaling);
			}
		}
	}
	return s_params;
}

structured_2d_field_data post::eval_slice(const sim& sim_instance, slice_plane slice, size_t port_num, size_t num_u, size_t num_v, double w)
{
	rectangle plane;
	if (slice == slice_plane::XY)
	{
		plane = rectangle(sim_instance.bbox.xmin, sim_instance.bbox.ymin,
			sim_instance.bbox.xmax, sim_instance.bbox.ymax);
	}
	else if (slice == slice_plane::XZ)
	{
		plane = rectangle(sim_instance.bbox.xmin, sim_instance.bbox.zmin,
			sim_instance.bbox.xmax, sim_instance.bbox.zmax);
	}
	else
	{
		plane = rectangle(sim_instance.bbox.ymin, sim_instance.bbox.zmin,
			sim_instance.bbox.ymax, sim_instance.bbox.zmax);
	}

	plane.add_padding(-1, -1);

	auto points = generate_grid_points(plane, num_u, num_v);
	structured_grid_2d grid(plane, num_u, num_v);
	Eigen::MatrixX2cd field(points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		const auto p = points[i];

		point_3d p3d;
		if (slice == slice_plane::XY)
		{
			p3d = { p.u, p.v, w };
		}
		else if (slice == slice_plane::XZ)
		{
			p3d = { p.u, w, p.v };
		}
		else
		{
			p3d = { w, p.u, p.v };
		}

		auto e = mesher_interface::get_volume_element_by_coordinate(p3d);
		if (!e.has_value())
		{
			field.row(i) << 0, 0;
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance.nodes,
			e.value(), p3d, sim_instance.full_dof_map, sim_instance.full_solutions[port_num]);

		if (slice == slice_plane::XY)
		{
			field.row(i) << elem_field(0), elem_field(1);
		}
		else if (slice == slice_plane::XZ)
		{
			field.row(i) << elem_field(0), elem_field(2);
		}
		else
		{
			field.row(i) << elem_field(1), elem_field(1);
		}
	}

	return { grid, field };
}