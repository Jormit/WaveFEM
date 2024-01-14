#include "sim.h"
#include "fem.h"

sim::sim(std::vector<node> nodes, std::vector<tet> volume_elems, std::vector<int> port_surface_ids, std::vector<std::vector<tri>> port_elems) :
	nodes(nodes), volume_elems(volume_elems), port_surface_ids(port_surface_ids), port_elems(port_elems),
	port_eigen_vectors(), port_eigen_wave_numbers()
{}

void sim::solve_ports()
{
	port_eigen_vectors.clear();
	port_eigen_wave_numbers.clear();
	for (int p = 0; p < port_elems.size(); p++)
	{
		auto dof_map = fem::_2d::mixed_order::dof_map(nodes, port_elems[p]);

		Eigen::SparseMatrix<double> S;
		Eigen::SparseMatrix<double> T;
		std::tie(S, T) = fem::_2d::mixed_order::assemble_S_T(nodes, port_elems[p], dof_map);

		Eigen::VectorXd values;
		Eigen::MatrixXd vecs;
		std::tie(values, vecs) = fem::solve_eigenproblem(S, T);

		port_eigen_wave_numbers.push_back(values.cwiseSqrt());
		port_eigen_vectors.push_back(vecs);
	}
}