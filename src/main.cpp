#include <iostream>
#include "mesher_interface.h"
#include "sim.h"
#include "setup.h"
#include "geometry.h"

#include <Eigen/dense>

int main()
{
	mesher_interface::initialize();

	int model_id = mesher_interface::import_model("../../../data/Horn.IGS");	
	auto port_points = setup::load_port_setup("../../../data/Horn.ports");
	
	auto bbox = mesher_interface::get_bounding_box();
	bbox.add_padding(20, 20, 20);
	int bbox_id = mesher_interface::add_box(bbox);

	mesher_interface::subtract(bbox_id, model_id);
	mesher_interface::mesh_model(5, 5);
	mesher_interface::view_model();
	
	auto nodes = mesher_interface::get_all_nodes();
	auto volume_elements = mesher_interface::get_all_volume_elems();

	auto port_entity_ids = mesher_interface::get_surface_from_com(port_points);
	auto port_surface_elements = mesher_interface::get_surface_elems_by_id(port_entity_ids);
	auto port_surface_dimensions = mesher_interface::get_surface_dimensions(port_entity_ids);

	sim sim(nodes, volume_elements, port_entity_ids, port_surface_elements);
	sim.solve_ports();	

	return 0;
}