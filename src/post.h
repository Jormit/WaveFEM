#pragma once
#include <memory>

#include "sim.h"

enum slice_plane {XY, XZ, YZ};

namespace post
{
	void eval_port(const sim& sim_instance, size_t num_x, size_t num_y);
	void eval_port(const sim& sim_instance, size_t port_num, size_t mode, size_t num_x, size_t num_y);

	void eval_full(const sim& sim_instance, size_t num_x, size_t num_y, size_t num_z);
	void eval_full(const sim& sim_instance, size_t port_num, size_t num_x, size_t num_y, size_t num_z);

	void eval_slice(const sim& sim_instance, slice_plane slice, size_t num_u, size_t num_v, double w);
	void eval_slice(const sim& sim_instance, slice_plane slice, size_t port_num, size_t num_u, size_t num_v, double w);
};