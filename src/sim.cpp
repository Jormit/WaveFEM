#include "sim.h"
#include "fem.h"
#include "mesher_interface.h"

#include <iostream>


sim::sim(std::vector<node> nodes, std::vector<tet> volume_elems, ports ports) :
	nodes(nodes), volume_elems(volume_elems), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps()
{
	mesher_interface::parameterize_surface_nodes(this->nodes, sim_ports.entity_ids, sim_ports.elements);
}

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

		std::cout << port_eigen_wave_numbers.back() << std::endl;
	}
}

std::vector<Eigen::Vector2d> sim::eval_port(size_t port_num, size_t num_x, size_t num_y)
{
	std::vector<Eigen::Vector2d> field;
	auto points = generate_grid_points({0, 0, 0, sim_ports.dimensions[port_num].width, sim_ports.dimensions[port_num].height, 0}, num_x, num_y, 1);
	for (const auto& p : points)
	{
		for (const auto& e : sim_ports.elements[port_num])
		{
			Eigen::Matrix<double, 3, 2> coords;
			coords <<
				nodes[e.nodes[0] - 1].parameterized_surface_point->u, nodes[e.nodes[0] - 1].parameterized_surface_point->v,
				nodes[e.nodes[1] - 1].parameterized_surface_point->u, nodes[e.nodes[1] - 1].parameterized_surface_point->v,
				nodes[e.nodes[2] - 1].parameterized_surface_point->u, nodes[e.nodes[2] - 1].parameterized_surface_point->v;

			auto coefficients = fem::_2d::simplex_coefficients(coords);
			Eigen::Vector2d coord;
			coord << p.x, p.y;

			auto lambda = fem::_2d::lambda(coord, coefficients);

			if (lambda(0) >= 0 && lambda(0) <= 1 &&
				lambda(1) >= 0 && lambda(1) <= 1 &&
				lambda(2) >= 0 && lambda(2) <= 1) {

				field.push_back(fem::_2d::mixed_order::eval_elem(nodes, e, { p.x, p.y }, port_dof_maps[port_num], port_eigen_vectors[port_num].col(0)));
				std::cout << p.x << " " << p.y << " ";
				std::cout << field.back().transpose() << std::endl;
				break;
			}
		}
	}
	return field;
}