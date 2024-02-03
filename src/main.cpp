#include <Eigen/dense>
#include <memory>
#include <iostream>

#include "mesher_interface.h"
#include "sim.h"
#include "geometry.h"
#include "ports.h"
#include "setup.h"
#include "helpers.h"
#include "material.h"
#include "post_processor.h"

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

	pml_id = mesher_interface::subtract(pml_id, boundary_id, false);
	boundary_id = mesher_interface::subtract(boundary_id, model_id);
	
	mesher_interface::mesh_model(20, 20);
	mesher_interface::view_model();	

	auto nodes = mesher_interface::get_all_nodes();	
	auto boundaries = mesher_interface::get_boundary_surfaces();
	mesher_interface::label_boundary_nodes(nodes, boundaries);

	auto elements = mesher_interface::get_volume_elems(boundary_id);
	auto pml_elements = mesher_interface::get_volume_elems(pml_id);
	
	auto base_materials = generate_base_material_set();
	label_pml_elements(pml_elements, boundary, nodes);
	elements.insert(elements.end(), pml_elements.begin(), pml_elements.end());

	ports ports(config.port_centres);
	ports.setup_port_nodes(nodes);

	auto current_sim = std::make_unique<sim>(pml, base_materials, nodes, elements, ports);

	current_sim->solve_ports();
	current_sim->solve_full(config.simulation_wavenumber);

	post_processor post(std::move(current_sim));

	post.eval_port(30, 30);
	post.eval_slice(slice_plane::YZ, 0, 100, 100, 0);
	post.eval_full(30, 30, 30);

	return 0;
}