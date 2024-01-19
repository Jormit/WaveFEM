#include "sim.h"
#include "fem.h"

#include <iostream>


sim::sim(std::vector<node> nodes, std::vector<tet> volume_elems, ports ports) :
	nodes(nodes), volume_elems(volume_elems), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps()
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
		std::tie(S, T) = fem::_2d::mixed_order::assemble_S_T(nodes, sim_ports.elements[p], dof_map, sim_ports.dimensions[p], sim_ports.entity_ids[p]);

		Eigen::VectorXd values;
		Eigen::MatrixXd vecs;
		std::tie(values, vecs) = fem::solve_eigenproblem(S, T);

		port_eigen_wave_numbers.push_back(values.cwiseSqrt());
		port_eigen_vectors.push_back(vecs);

		std::cout << port_eigen_wave_numbers.back() << std::endl;
	}
}