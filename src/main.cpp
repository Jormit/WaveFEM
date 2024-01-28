#include <iostream>
#include "mesher_interface.h"
#include "sim.h"
#include "geometry.h"
#include "ports.h"
#include "setup.h"

#include <Eigen/dense>

const std::string data_path = "../../../data/";

int main()
{
	setup config(data_path + "config.json");

	mesher_interface::initialize();

	int model_id = mesher_interface::import_model(data_path + config.model_file);

	auto boundary = mesher_interface::get_bounding_box();
	boundary.add_padding(config.bounding_box_padding);
	int boundary_id = mesher_interface::add_box(boundary);

	auto pml = boundary;
	pml.add_padding(config.pml_thickness);
	int pml_id = mesher_interface::add_box(pml);

	mesher_interface::subtract(pml_id, boundary_id, false);
	mesher_interface::subtract(boundary_id, model_id);
	
	mesher_interface::mesh_model(20, 20);
	mesher_interface::view_model();	

	auto nodes = mesher_interface::get_all_nodes();
	auto volume_elements = mesher_interface::get_all_volume_elems();

	ports ports(config.port_centres);
	sim sim(boundary, nodes, volume_elements, ports);

	sim.solve_ports();
	sim.solve_full(config.simulation_wavenumber);

	sim.eval_port(30, 30);
	sim.eval_xslice(0, 100, 100, 0);

	return 0;
}