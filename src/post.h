#pragma once
#include <memory>
#include <Eigen/Dense>
#include <utility>
#include <vector>

#include "sim.h"
#include "geometry.h"

enum slice_plane {XY, XZ, YZ};
enum field_type {E_FIELD, H_FIELD, POYNTING};

namespace post
{
	Eigen::Vector3cd eval_field_at_point(const sim& sim_instance, point_3d point, size_t port_num, field_type type);

	structured_2d_field_data eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y);
	structured_2d_field_data eval_port_from_3d(const sim& sim_instance, size_t eval_port_num, size_t driven_port_num, size_t num_x, size_t num_y);
	structured_3d_field_data eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z, field_type type);

	Eigen::MatrixXcd eval_s_parameters(const sim& sim_instance, size_t num_x, size_t num_y);
};