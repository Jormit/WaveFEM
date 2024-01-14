#include <iostream>
#include "mesher_interface.h"
#include "fem.h"
#include "setup.h"

int main()
{
	mesher_interface::initialize();

	int model_id = mesher_interface::import_model("../../../data/Horn.IGS");	
	auto port_points = setup::load_port_setup("../../../data/Horn.ports");
	
	auto bbox = mesher_interface::get_bounding_box();
	bbox.add_padding(20, 20, 20);
	int bbox_id = mesher_interface::add_box(bbox);

	mesher_interface::subtract(bbox_id, model_id);
	mesher_interface::mesh_model(10, 10);
	mesher_interface::view_model();
	
	auto nodes = mesher_interface::get_all_nodes();
	auto volume_elements = mesher_interface::get_volume_elems();

	auto port_ids = mesher_interface::get_surface_ids_from_coms(port_points);
	auto port_surface_elements = mesher_interface::get_surface_elems_by_ids(port_ids);

	auto dof_map = fem::_2d::mixed_order::dof_map(nodes, port_surface_elements[0]);

	Eigen::SparseMatrix<double> S;
	Eigen::SparseMatrix<double> T;
	std::tie(S, T) = fem::_2d::mixed_order::assemble_S_T(nodes, port_surface_elements[0], dof_map);

	//fem::solve_eigenproblem(S, T);

	return 0;
}