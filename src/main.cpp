#include <iostream>
#include "mesher_interface.h"
#include "sim.h"
#include "geometry.h"
#include "ports.h"

#include <Eigen/dense>

int main()
{
	mesher_interface::initialize();

	int model_id = mesher_interface::import_model("../../../data/Horn.STEP");
	auto bbox = mesher_interface::get_bounding_box();
	bbox.add_padding(20, 20, 20);
	int bbox_id = mesher_interface::add_box(bbox);

	mesher_interface::subtract(bbox_id, model_id);
	mesher_interface::mesh_model(20, 20);
	mesher_interface::view_model();

	auto nodes = mesher_interface::get_all_nodes();
	auto volume_elements = mesher_interface::get_all_volume_elems();

	ports ports(std::string("../../../data/Horn.ports"));

	sim sim(bbox, nodes, volume_elements, ports);
	sim.solve_ports();
	sim.eval_port(0, 30, 30);

	sim.solve_full();
	sim.eval_xslice(0, 100, 100, 0);
	return 0;
}