#pragma once
#include <memory>

#include "sim.h"

enum slice_plane {XY, XZ, YZ};

class post_processor
{
public:
	post_processor(std::unique_ptr<sim> sim_instance);

	void eval_port(size_t num_x, size_t num_y);
	void eval_port(size_t port_num, size_t mode, size_t num_x, size_t num_y);

	void eval_full(size_t num_x, size_t num_y, size_t num_z);
	void eval_full(size_t port_num, size_t num_x, size_t num_y, size_t num_z);

	void eval_slice(slice_plane plane, size_t port_num, size_t num_u, size_t num_v, double w);

private:
	std::unique_ptr<sim> sim_instance;
};