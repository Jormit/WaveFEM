#include "sim.h"
#include "fem.h"
#include "mesher_interface.h"
#include "helpers.h"

#include <complex>
#include <iostream>
#include <fstream>
#include <format>


sim::sim(box bbox, std::vector<node> nodes, std::vector<tet> volume_elems, ports ports) :
	bbox(bbox), nodes(nodes), volume_elems(volume_elems), sim_ports(ports),
	port_eigen_vectors(), port_eigen_wave_numbers(), port_dof_maps(),
	full_solutions(), full_dof_map()
{
	mesher_interface::parameterize_surface_nodes(this->nodes, sim_ports.entity_ids, sim_ports.elements);
	sim_ports.update_node_tags(this->nodes);
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

void sim::solve_full()
{
	full_solutions.clear();
	full_dof_map.clear();
	double k = 0.06;

	full_dof_map = fem::_3d::mixed_order::dof_map(nodes, volume_elems);
	auto surface_elems = helpers::flatten_vector<tri>(sim_ports.elements);
	auto A = fem::_3d::mixed_order::assemble_A(nodes, volume_elems, surface_elems, full_dof_map, { k, 0 }, { 0, k });

	Eigen::SparseLU<Eigen::SparseMatrix<std::complex<double>>, Eigen::COLAMDOrdering<int>> solver;
	solver.analyzePattern(A);
	solver.factorize(A);

	for (int p = 0; p < sim_ports.elements.size(); p++)
	{
		auto b = fem::_3d::mixed_order::assemble_b(nodes, sim_ports.elements[p], full_dof_map, port_dof_maps[p], port_eigen_vectors[p], k);
		full_solutions.push_back(solver.solve(b));
	}
}

void sim::eval_port(size_t port_num, size_t num_x, size_t num_y)
{
	auto bounds = sim_ports.bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	std::ofstream ofs(std::format("port_{}.txt", port_num));

	for (const auto& p : points)
	{
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_ports.entity_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(nodes, e, { p.u, p.v }, port_dof_maps[port_num], port_eigen_vectors[port_num].col(0));
		ofs << p.u << " " << p.v << " ";
		ofs << elem_field(0) << " " << elem_field(1) << std::endl;
	}
}

void sim::eval_full(size_t port_num, size_t num_x, size_t num_y, size_t num_z)
{	
	auto points = generate_grid_points(bbox, num_x, num_y, num_z);

	std::ofstream ofs(std::format("full_port{}.txt", port_num));

	for (const auto& p : points)
	{
		auto e = mesher_interface::get_volume_element_by_coordinate(p);
		auto elem_field = fem::_3d::mixed_order::eval_elem(nodes, e, p, full_dof_map, full_solutions[port_num]);

		ofs << p.x << " " << p.y << " " << p.z << " ";

		ofs << elem_field(0).real();		
		if (elem_field(0).imag() > 0) ofs << "+";
		ofs << elem_field(0).imag() << "i ";

		ofs << elem_field(1).real();
		if (elem_field(1).imag() > 0) ofs << "+";
		ofs << elem_field(1).imag() << "i ";

		ofs << elem_field(2).real();
		if (elem_field(2).imag() > 0) ofs << "+";
		ofs << elem_field(2).imag() << "i ";

		ofs << std::endl;
	}
}

void sim::eval_xslice(size_t port_num, size_t num_x, size_t num_y, double x)
{
	rectangle plane(bbox.ymin,  bbox.zmin, bbox.ymax, bbox.zmax );
	auto points = generate_grid_points(plane, num_x, num_y);

	std::ofstream ofs(std::format("full_port{}.txt", port_num));

	for (const auto& p : points)
	{
		point_3d p3d = { x, p.u, p.v };
		auto e = mesher_interface::get_volume_element_by_coordinate(p3d);
		auto elem_field = fem::_3d::mixed_order::eval_elem(nodes, e, p3d, full_dof_map, full_solutions[port_num]);

		ofs << p3d.x << " " << p3d.y << " " << p3d.z << " ";

		ofs << elem_field(0).real();
		if (elem_field(0).imag() > 0) ofs << "+";
		ofs << elem_field(0).imag() << "i ";

		ofs << elem_field(1).real();
		if (elem_field(1).imag() > 0) ofs << "+";
		ofs << elem_field(1).imag() << "i ";

		ofs << elem_field(2).real();
		if (elem_field(2).imag() > 0) ofs << "+";
		ofs << elem_field(2).imag() << "i ";

		ofs << std::endl;
	}
}
