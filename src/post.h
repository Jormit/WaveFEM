#pragma once
#include <memory>
#include <Eigen/Dense>
#include <utility>
#include <vector>

#include "sim.h"

enum slice_plane {XY, XZ, YZ};

namespace post
{
	std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd> eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y);
	std::pair<Eigen::MatrixX2d, Eigen::MatrixX2cd> eval_port_from_3d(const sim& sim_instance, size_t eval_port_num, size_t driven_port_num, size_t num_x, size_t num_y);
	std::pair<Eigen::MatrixX3d, Eigen::MatrixX3cd> eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z);

	Eigen::MatrixXcd eval_s_parameters(const sim& sim_instance, size_t num_x, size_t num_y);
};