#include <Eigen/dense>
#include <iostream>
#include <cassert>

#include "mesher_interface.h"
#include "sim.h"
#include "geometry.h"
#include "ports.h"
#include "config.h"
#include "helpers.h"
#include "material.h"
#include "post.h"
#include "pml.h"
#include "result_writer.h"

const std::string data_path = "../../../data/";

int main()
{
	mesher_interface::initialize();
	sim_config config(data_path + "waveguide with obstacle.json");
	auto model_ids = mesher_interface::import_model(data_path + config.model_file);

	auto boundary = mesher_interface::get_bounding_box();
	boundary.add_padding(config.bounding_box_padding);
	int boundary_id = mesher_interface::add_box(boundary);

	std::vector<int> free_space_volumes{ boundary_id };

	for (size_t i = 0; i < model_ids.size(); i++)
	{
		auto material_id = config.material_assignments[i];
		if (config.materials[material_id].PEC)
		{
			free_space_volumes = mesher_interface::subtract(free_space_volumes, model_ids[i], true);
		}
		else
		{
			free_space_volumes = mesher_interface::subtract(free_space_volumes, model_ids[i], false);
		}
	}

	pml_boundary pml;

	if (config.pml_enable)
	{
		pml = pml::create(config.pml_thickness);
	}	

	mesher_interface::mesh_model(20, 20);
	mesher_interface::view_model();

	auto base_materials = mat::generate_base_set();

	auto nodes = mesher_interface::get_all_nodes();
	auto boundaries = mesher_interface::get_boundary_surfaces();

	std::unordered_set<size_t> boundary_edges;
	std::unordered_set<size_t> boundary_faces;
	std::tie(boundary_edges, boundary_faces) = mesher_interface::get_surface_edges_and_faces(boundaries);

	auto boundary_edge_map = helpers::set_to_map<size_t, int>(boundary_edges, BOUNDARY);
	auto boundary_face_map = helpers::set_to_map<size_t, int>(boundary_faces, BOUNDARY);

	auto elements = helpers::flatten_vector(mesher_interface::get_volume_elems(free_space_volumes));

	if (config.pml_enable)
	{
		auto pml_elements = pml::get_elements(pml);
		elements.insert(elements.end(), pml_elements.begin(), pml_elements.end());
	}

	for (size_t i = 0; i < model_ids.size(); i++)
	{
		auto material_id = config.material_assignments[i];
		if (!config.materials[material_id].PEC)
		{
			auto mat_elements = mesher_interface::get_volume_elems(i + 1);
			elements.insert(elements.end(), mat_elements.begin(), mat_elements.end());
		}
	}

	ports ports(config.port_centres);
	ports.setup_port_nodes(nodes);
	ports.setup_port_faces_and_edges(boundary_edge_map, boundary_face_map);

	sim current_sim (boundary, base_materials, nodes,
		elements, boundary_edge_map, boundary_face_map, ports);

	current_sim.solve_ports();
	current_sim.solve_full(config.simulation_wavenumber);

	// Output currently for debug

	auto port_1_excitation = post::eval_port(current_sim, 0, 0, 30, 30);
	auto face_sol = post::eval_slice(current_sim, slice_plane::XY, 0, 32, 32, boundary.zmax);
	auto full_sol = post::eval_full(current_sim, 0, 30, 30, 30);	

	result_writer::write_2d_field("Port Solution 2d.txt", port_1_excitation);
	result_writer::write_2d_field("Slice Solution 2d.txt", face_sol);
	result_writer::write_3d_field("Full Solution.txt", full_sol);

	auto s_params = post::eval_s_parameters(current_sim, 30, 30);
	std::cout << s_params << std::endl;

	return 0;
}