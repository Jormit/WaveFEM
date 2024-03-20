#include <iostream>

#include "mesher_interface.h"
#include "sim.h"
#include "config.h"
#include "post.h"
#include "result_writer.h"
#include "argument_parser.h"

int main(int argc, char* argv[])
{
	arg_parser args(argc, argv);
	sim_config config(args.data_path + args.config_filename);

	auto current_sim = sim::create (config, args.data_path);

	mesher_interface::view_model();

	current_sim.solve_ports(config.simulation_wavenumber);
	current_sim.solve_full(config.simulation_wavenumber);

	auto port_1_excitation = post::eval_port(current_sim, 0, 1, 30, 30);
	auto face_sol = post::eval_slice(current_sim, slice_plane::XY, 0, 32, 32, current_sim.bbox.zmax);
	auto full_sol = post::eval_full(current_sim, 0, 30, 30, 30);

	result_writer::write_2d_field("Port Solution 2d.txt", port_1_excitation);
	result_writer::write_2d_field("Slice Solution 2d.txt", face_sol);
	result_writer::write_3d_field("Full Solution.txt", full_sol);

	auto s_params = post::eval_s_parameters(current_sim, 30, 30);
	std::cout << s_params << std::endl;

	return 0;
}