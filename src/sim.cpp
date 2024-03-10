#include <complex>
#include <iostream>

#include "sim.h"
#include "fem.h"
#include "mesher_interface.h"
#include "helpers.h"
#include "pml.h"

sim::sim(box bbox, std::vector<material> materials, std::vector<node> nodes, std::vector<tet> volume_elems,
	std::unordered_map<size_t, int> boundary_edge_map, std::unordered_map<size_t, int> boundary_face_map, ports ports) :
	bbox(bbox), materials(materials), nodes(nodes), volume_elems(volume_elems),
	boundary_edge_map(boundary_edge_map), boundary_face_map(boundary_face_map), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps(),
	full_solutions(), full_dof_map()
{}

sim sim::create(sim_config config, std::string data_path)
{
	mesher_interface::initialize();
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
			free_space_volumes
				= mesher_interface::subtract(free_space_volumes, model_ids[i], true);
		}
		else
		{
			free_space_volumes
				= mesher_interface::subtract(free_space_volumes, model_ids[i], false);
		}
	}

	pml_boundary pml;

	if (config.pml_enable)
	{
		pml = pml::create(config.pml_thickness);
	}

	mesher_interface::mesh_model(
		config.target_mesh_size * 0.9,
		config.target_mesh_size * 1.1
	);

	auto base_materials = mat::generate_base_set();
	auto user_materials = mat::load_user_materials(config.materials);

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
			mat::label_elems(mat_elements, base_materials.size() + i);
			elements.insert(elements.end(), mat_elements.begin(), mat_elements.end());
		}
	}

	base_materials.insert(base_materials.end(), user_materials.begin(), user_materials.end());

	ports ports(config.port_centres);
	ports.setup_port_nodes(nodes);
	ports.setup_port_faces_and_edges(boundary_edge_map, boundary_face_map);

	return {
		std::move(boundary),
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
	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		auto dof_map = fem::_2d::mixed_order::dof_map(sim_ports.elements[p], boundary_edge_map);
		port_dof_maps.push_back(dof_map);

		Eigen::SparseMatrix<double> S;
		Eigen::SparseMatrix<double> T;
		std::tie(S, T) = fem::_2d::mixed_order::assemble_S_T(nodes, sim_ports.elements[p], dof_map);

		Eigen::VectorXd values;
		Eigen::MatrixXd vecs;
		std::tie(values, vecs) = fem::solve_eigenproblem(S, T);

		port_eigen_wave_numbers.push_back(values.cwiseSqrt());
		port_eigen_vectors.push_back(vecs);
	}
}

void sim::solve_full(double k)
{
	full_solutions.clear();
	full_dof_map.clear();

	full_dof_map = fem::_3d::mixed_order::dof_map(volume_elems, boundary_edge_map, boundary_face_map);
	auto surface_elems = helpers::flatten_vector<tri>(sim_ports.elements);

	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		std::complex<double> k_inc = std::sqrt(
			static_cast<std::complex<double>>(k * k) -
			port_eigen_wave_numbers[p](0) * port_eigen_wave_numbers[p](0)
		);
		auto A = fem::_3d::mixed_order::assemble_A(
			nodes,
			volume_elems,
			materials,
			surface_elems,
			full_dof_map,
			k,
			k_inc * std::complex<double>{0, 1}
		);
		Eigen::SparseLU<Eigen::SparseMatrix<std::complex<double>>, Eigen::COLAMDOrdering<int>> solver;
		solver.analyzePattern(A);
		solver.factorize(A);

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
}
