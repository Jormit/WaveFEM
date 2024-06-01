#include <iostream>
#include <cmath>

#include "post.h"
#include "mesher_interface.h"
#include "fem.h"
#include "helpers.h"
#include "constants.h"

Eigen::Vector3cd post::eval_field_at_point(const sim& sim_instance, geo::point_3d point, size_t port_num, field_type type)
{
	Eigen::Vector3cd e_field;
	Eigen::Vector3cd h_field;

	auto element_id = mesher_interface::get_volume_element_by_coordinate(point);
	
	if (!element_id.has_value())
	{
		e_field << 0, 0, 0;
		return e_field;
	}

	auto e = sim_instance.volume_elems[element_id.value()];

	if (type == field_type::E_FIELD || type == field_type::POYNTING)
	{
		e_field = fem::_3d::mixed_order::eval_elem(sim_instance.nodes,
			e, point, sim_instance.full_dof_map, sim_instance.full_solutions[port_num]);
	}

	if (type == field_type::H_FIELD || type == field_type::POYNTING)
	{
		auto mat = sim_instance.materials[e.material_id];
		auto mu_inv = mat.permeability.inverse();
		h_field = std::complex<double>{ 0, 1 } * mu_inv / (constants::mu_0 * constants::k2omega * sim_instance.wavenumber) *
			fem::_3d::mixed_order::eval_elem_curl(sim_instance.nodes, e, point,
				sim_instance.full_dof_map, sim_instance.full_solutions[port_num]);
	}

	if (type == field_type::E_FIELD)
	{
		return e_field;
	}

	else if (type == field_type::H_FIELD)
	{
		return h_field;
	}

	return e_field.cross(h_field.conjugate());
}

Eigen::Vector2cd post::eval_surface_field_at_parametric_point(Eigen::VectorXcd solution,
	fem::dof_map dof_map, std::vector<geo::node> nodes, int surface_id, geo::point_2d point)
{
	auto e = mesher_interface::get_surface_element_by_parametric_coordinate(point, surface_id);
	return fem::_2d::mixed_order::eval_elem(nodes, e, { point.u, point.v }, dof_map, solution);
}

geo::structured_2d_field_data post::eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.parametric_bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	geo::structured_grid_2d grid(bounds, num_x, num_y);
	Eigen::MatrixX2cd field (points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		field.row(i) = eval_surface_field_at_parametric_point(
			sim_instance.port_eigen_vectors[port_num].col(mode),
			sim_instance.port_dof_maps[port_num],
			sim_instance.nodes,
			sim_instance.sim_ports.dummy_ids[port_num],
			points[i]);
	}

	return { grid, field };
}

geo::structured_2d_field_data post::eval_port_from_3d(const sim& sim_instance, size_t eval_port_num,
	size_t driven_port_num, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance.sim_ports.parametric_bounds[eval_port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	geo::structured_grid_2d grid(bounds, num_x, num_y);
	Eigen::MatrixX2cd field(points.size(), 2);

	for (size_t i = 0; i < points.size(); i++)
	{
		field.row(i) = eval_surface_field_at_parametric_point(
			sim_instance.full_solutions[driven_port_num],
			sim_instance.full_dof_map,
			sim_instance.nodes,
			sim_instance.sim_ports.dummy_ids[eval_port_num],
			points[i]);
	}

	return { grid, field };
}

geo::structured_3d_field_data post::eval_full(const sim& sim_instance, size_t port_num,
	size_t num_x, size_t num_y, size_t num_z, field_type type)
{
	auto points = geo::generate_grid_points(sim_instance.bbox, num_x, num_y, num_z);

	geo::structured_grid_3d grid(sim_instance.bbox, num_x, num_y, num_z);
	Eigen::MatrixX3cd field(points.size(), 3);

	for (size_t i = 0; i < points.size(); i++)
	{
		field.row(i) = eval_field_at_point(sim_instance, points[i], port_num, type);
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

geo::unstructured_3d_field_data post::project_2d_structured_surface_field_into_3d(
	geo::structured_2d_field_data data, int surface_id)
{
	auto points = generate_grid_points(data.grid);

	Eigen::MatrixX3d out_points(points.size(), 3);
	Eigen::MatrixX3cd out_field(points.size(), 3);	

	for (size_t i = 0; i < points.size(); i++)
	{
		auto point = mesher_interface::evaluate_surface_parameterization(surface_id, points[i]);
		auto deriv = mesher_interface::evaluate_surface_derivative(surface_id, points[i]);

		out_points.row(i) = point.to_Eigen();

		Eigen::Vector2cd field_vec{ data.field.row(i) };
		out_field.row(i) = deriv * field_vec;
	}
	return { out_points, out_field };
}