#include <complex>
#include <iostream>

#include "sim.h"
#include "fem.h"
#include "mesher_interface.h"
#include "helpers.h"
#include "result_formatter.h"

sim::sim(box bbox, std::vector<material> materials, std::vector<node> nodes, std::vector<tet> volume_elems,
	std::unordered_set<size_t> boundary_edges, std::unordered_set<size_t> boundary_faces, ports ports) :
	bbox(bbox), materials(materials), nodes(nodes), volume_elems(volume_elems),
	boundary_edges(boundary_edges), boundary_faces(boundary_faces), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps(),
	full_solutions(), full_dof_map()
{}

void sim::solve_ports()
{
	port_eigen_vectors.clear();
	port_eigen_wave_numbers.clear();
	port_dof_maps.clear();
	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		auto dof_map = fem::_2d::mixed_order::dof_map(nodes, sim_ports.elements[p]);
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

	full_dof_map = fem::_3d::mixed_order::dof_map(nodes, volume_elems, boundary_edges, boundary_faces);
	auto surface_elems = helpers::flatten_vector<tri>(sim_ports.elements);

	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		std::complex<double> k_inc = std::sqrt(static_cast<std::complex<double>>(k * k) - port_eigen_wave_numbers[p](0) * port_eigen_wave_numbers[p](0));
		auto A = fem::_3d::mixed_order::assemble_A(nodes, volume_elems, materials, surface_elems, full_dof_map, k, k_inc * std::complex<double>{0, 1});
		Eigen::SparseLU<Eigen::SparseMatrix<std::complex<double>>, Eigen::COLAMDOrdering<int>> solver;
		solver.analyzePattern(A);
		solver.factorize(A);

		auto b = fem::_3d::mixed_order::assemble_b(nodes, sim_ports.elements[p], full_dof_map, port_dof_maps[p], port_eigen_vectors[p].col(0), k_inc);
		full_solutions.push_back(solver.solve(-b));
	}
}
