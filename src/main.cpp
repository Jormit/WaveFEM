#include <iostream>
#include <filesystem>

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

	// mesher_interface::view_model();

	current_sim.solve_ports(config.simulation_wavenumber);
	current_sim.solve_full(config.simulation_wavenumber);

	std::string outputs_dir = args.data_path + args.raw_config_filename + "_outputs/";
	std::filesystem::create_directory(outputs_dir);
	current_sim.generate_outputs(outputs_dir, config.simulation_wavenumber, config);

	return 0;
}