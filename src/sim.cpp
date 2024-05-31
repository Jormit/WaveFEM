#pragma once

#include <complex>
#include <iostream>
#include <unordered_map>
#include <format>

#include "sim.h"
#include "mesher_interface.h"
#include "helpers.h"
#include "pml.h"
#include "result_writer.h"

sim::sim(box bbox, double wavenumber, std::vector<material> materials, std::vector<node> nodes, std::vector<tet> volume_elems,
	std::unordered_map<size_t, int> boundary_edge_map, std::unordered_map<size_t, int> boundary_face_map, ports ports) :
	bbox(bbox), wavenumber(wavenumber), materials(materials), nodes(nodes), volume_elems(volume_elems),
	boundary_edge_map(boundary_edge_map), boundary_face_map(boundary_face_map), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps(),
	full_solutions(), full_dof_map()
{}

sim sim::create(sim_config config, std::string data_path)
{
	// Import step file
	mesher_interface::initialize();
	auto model_ids = mesher_interface::import_step(data_path + config.model_file);

	// Create bounding box with user specified padding
	auto boundary = mesher_interface::get_bounding_box();
	boundary.add_padding(config.bounding_box_padding);
	int boundary_id = mesher_interface::add_box(boundary);

	// Generate conformal simulation volumes.
	// This is handled by 2 cases:
	// 1. Conductor objects are subtracted from bounding box and then removed.
	// 2. Dielectric objects are substracted from bounding box and then kept.
	std::vector<int> free_space_volumes{ boundary_id };
	for (size_t i = 0; i < model_ids.size(); i++)
	{
		auto material_id = config.material_assignments[i];
		if (config.materials[material_id].PEC)
		{
			free_space_volumes
				= mesher_interface::subtract(free_space_volumes, model_ids[i], true);
		}
		else
		{
			free_space_volumes
				= mesher_interface::subtract(free_space_volumes, model_ids[i], false);
		}
	}

	// Create the PML if specified
	pml_boundary pml;
	if (config.pml_enable)
	{
		pml = pml::create(config.pml_thickness);
	}

	// Merge all conformal surfaces
	mesher_interface::remove_duplicates();

	mesher_interface::mesh_model(
		config.target_mesh_size * 0.9,
		config.target_mesh_size * 1.1
	);

	// Create materials.
	auto base_materials = mat::generate_base_set();
	auto user_materials = mat::load_user_materials(config.materials);

	// Get all nodes and boundary surfaces. Then mark which edges are on boundary.
	auto nodes = mesher_interface::get_all_nodes();
	auto boundaries = mesher_interface::get_boundary_surfaces();

	std::unordered_set<size_t> boundary_edges;
	std::unordered_set<size_t> boundary_faces;
	std::tie(boundary_edges, boundary_faces) = mesher_interface::get_surface_edges_and_faces(boundaries);

	auto boundary_edge_map = helpers::set_to_map<size_t, int>(boundary_edges, BOUNDARY);
	auto boundary_face_map = helpers::set_to_map<size_t, int>(boundary_faces, BOUNDARY);

	// Get all elements and label material
	auto elements = helpers::flatten_vector(mesher_interface::get_volume_elems(free_space_volumes));

	if (config.pml_enable)
	{
		auto pml_elements = pml::get_elements(pml);
		elements.insert(elements.end(), pml_elements.begin(), pml_elements.end());
	}

	std::unordered_map<size_t, size_t> volume_material_map;
	for (size_t i = 0; i < model_ids.size(); i++)
	{
		auto material_id = config.material_assignments[i];
		if (!config.materials[material_id].PEC)
		{
			size_t volume_id = i + 1;
			material_id = base_materials.size() + material_id - 1;
			auto mat_elements = mesher_interface::get_volume_elems(volume_id);
			mat::label_elems(mat_elements, material_id);
			volume_material_map[volume_id] = material_id;
			elements.insert(elements.end(), mat_elements.begin(), mat_elements.end());
		}
	}

	base_materials.insert(base_materials.end(), user_materials.begin(), user_materials.end());

	// Setup ports
	ports ports(config.port_bounding_boxes, volume_material_map);
	ports.setup_port_nodes_faces_edges(nodes, boundary_edge_map, boundary_face_map);

	auto bounding_box_surfaces = mesher_interface::get_bounding_box_surfaces(boundary);

	return {
		std::move(boundary),
		config.simulation_wavenumber,
		std::move(base_materials),
		std::move(nodes),
		std::move(elements),
		std::move(boundary_edge_map),
		std::move(boundary_face_map),
		std::move(ports)
	};
}

void sim::solve_ports()
{
	port_eigen_vectors.clear();
	port_eigen_wave_numbers.clear();
	port_dof_maps.clear();
	for (int p = 0; p < sim_ports.num_ports(); p++)
	{
		// Generate port matrix
		auto dof_map = fem::_2d::mixed_order::generate_dof_map(nodes, sim_ports.elements[p], boundary_edge_map);
		port_dof_maps.push_back(dof_map);
		Eigen::SparseMatrix<double> A;
		Eigen::SparseMatrix<double> B;
		std::tie(A, B) = fem::_2d::mixed_order::assemble_A_B(nodes, sim_ports.elements[p], materials, dof_map, wavenumber);

		// Solve eigenproblem
		Eigen::VectorXd values;
		Eigen::MatrixXd vecs;
		std::tie(values, vecs) = fem::solve_eigenproblem(A, -B, 1e-4);

		// Save results
		port_eigen_wave_numbers.push_back(values.cwiseSqrt());
		port_eigen_vectors.push_back(vecs);

		std::cout << port_eigen_wave_numbers.back() << std::endl;
	}
}

void sim::solve_full()
{
	full_solutions.clear();
	full_dof_map.clear();

	// Assume first order mode termination
	std::vector<std::complex<double>> boundary_k;
	for (int p = 0; p < sim_ports.num_ports(); p++)
	{
		boundary_k.push_back(port_eigen_wave_numbers[p](0));
	}

	// Generate system matrix and factorize
	full_dof_map = fem::_3d::mixed_order::generate_dof_map(volume_elems, boundary_edge_map, boundary_face_map);
	auto A = fem::_3d::mixed_order::assemble_A(
		nodes,
		volume_elems,
		materials,
		sim_ports.elements,
		full_dof_map,
		wavenumber,
		boundary_k
	);
	Eigen::SparseLU<Eigen::SparseMatrix<std::complex<double>>, Eigen::COLAMDOrdering<int>> solver;
	solver.analyzePattern(A);
	solver.factorize(A);

	// Solve for all port excitations
	for (int p = 0; p < sim_ports.num_ports(); p++)
	{
		std::complex<double> k_inc = boundary_k[p];

		auto b = fem::_3d::mixed_order::assemble_b(
			nodes,
			sim_ports.elements[p],
			full_dof_map,
			port_dof_maps[p],
			port_eigen_vectors[p].col(0),
			k_inc
		);		
		full_solutions.push_back(solver.solve(-b));
	}

	// result_writer::write_mtx("mat.mtx", A);
}

void sim::generate_outputs(std::string directory, sim_config config)
{
	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		// Save port fields
		auto filename = std::format("Port {} Field", p);

		auto port_excitation = post::eval_port(*this, p, 0, 20, 20);
		auto port_excitation_3d = 
			post::project_2d_structured_surface_field_into_3d(port_excitation, sim_ports.dummy_ids[p]);
		result_writer::write_unstructured_3d_field(directory + filename, port_excitation_3d);

		// Save 3d fields
		auto E_filename = std::format("E Field [Port {}]", p);
		auto B_filename = std::format("H Field [Port {}]", p);

		auto e_field = post::eval_full(*this, p,
			static_cast<int> (bbox.x_dim() / config.target_mesh_size * 3),
			static_cast<int> (bbox.y_dim() / config.target_mesh_size * 3),
			static_cast<int> (bbox.z_dim() / config.target_mesh_size * 3), field_type::E_FIELD);

		auto h_field = post::eval_full(*this, p,
			static_cast<int> (bbox.x_dim() / config.target_mesh_size * 3),
			static_cast<int> (bbox.y_dim() / config.target_mesh_size * 3),
			static_cast<int> (bbox.z_dim() / config.target_mesh_size * 3), field_type::H_FIELD);

		result_writer::write_structured_3d_field(directory + E_filename, e_field);
		result_writer::write_structured_3d_field(directory + B_filename, h_field);
	}

	// Print s-params
	auto s_params = post::eval_s_parameters(*this, 30, 30);
	std::cout << s_params << std::endl;

	// Save mesh
	mesher_interface::write_vtk(directory + "mesh");
}
