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
#include "boundary.h"

const std::string data_path = "../../../data/";

int main()
{
	setup config(data_path + "config waveguide open.json");

	mesher_interface::initialize();

	int model_id = mesher_interface::import_model(data_path + config.model_file);

	auto boundary = mesher_interface::get_bounding_box();
	boundary.add_padding(config.bounding_box_padding);
	int boundary_id = mesher_interface::add_box(boundary);
	auto free_space_volumes = mesher_interface::subtract(boundary_id, model_id);

	auto pml_ids = boundary::setup_pml_boundary(config.pml_thickness);

	mesher_interface::mesh_model(20, 20);
	mesher_interface::view_model();

	auto nodes = mesher_interface::get_all_nodes();
	auto boundaries = mesher_interface::get_boundary_surfaces();
	mesher_interface::label_boundary_nodes(nodes, boundaries);

	std::unordered_set<size_t> boundary_edges;
	std::unordered_set<size_t> boundary_faces;
	std::tie(boundary_edges, boundary_faces) = mesher_interface::get_boundary_edges_and_faces(boundaries);

	auto elements = helpers::flatten_vector(mesher_interface::get_volume_elems(free_space_volumes));
	auto pml_elements = boundary::get_pml_elements(pml_ids);
	elements.insert(elements.end(), pml_elements.begin(), pml_elements.end());

	auto base_materials = mat::generate_base_set();

	ports ports(config.port_centres);
	ports.setup_port_nodes(nodes);

	auto current_sim = std::make_unique<sim>(mesher_interface::get_bounding_box(),
		base_materials, nodes, elements, boundary_edges, boundary_faces, ports);

	current_sim->solve_ports();
	current_sim->solve_full(config.simulation_wavenumber);

	post_processor post(std::move(current_sim));

	post.eval_port(30, 30);
	post.eval_slice(slice_plane::YZ, 100, 100, 0);
	post.eval_full(30, 30, 30);
	post.eval_s_parameters();

	return 0;
}