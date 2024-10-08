#pragma once
#include <memory>
#include <Eigen/Dense>
#include <utility>
#include <vector>

#include "sim.h"
#include "geometry.h"

enum field_type {E_FIELD, H_FIELD, POYNTING};

namespace post
{
	Eigen::Vector3cd eval_field_at_point(const sim& sim_instance, geo::point_3d point, size_t port_num, field_type type);
	Eigen::Vector2cd eval_surface_field_at_parametric_point(Eigen::VectorXcd solution,
		fem::dof_map dof_map, std::vector<geo::node> nodes, int surface_id, geo::point_2d point);

	geo::structured_2d_field_data eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y);
	geo::structured_2d_field_data eval_port_from_3d(const sim& sim_instance, size_t eval_port_num, size_t driven_port_num, size_t num_x, size_t num_y);
	geo::structured_3d_field_data eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z, field_type type);

	Eigen::MatrixXcd eval_s_parameters(const sim& sim_instance, size_t num_x, size_t num_y);

	geo::unstructured_3d_field_data project_2d_structured_surface_field_into_3d(geo::structured_2d_field_data data, int surface_id);

	geo::polar_2d_field_data eval_far_field_slice(sim& sim_instance, size_t port_num, size_t num, geo::far_field_slice type, double fixed_angle, double r);
};