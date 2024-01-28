#pragma once
#include <vector>
#include <Eigen/Dense>
#include <map>

#include "ports.h"
#include "geometry.h"


// This class manages all the elements within a simulation context
class sim
{
public:
	sim(box bbox, std::vector<node> nodes, std::vector<tet> volume_elems, ports ports);

	void solve_ports();
	void solve_full(double k);

	void eval_port(size_t num_x, size_t num_y);
	void eval_port(size_t port_num, size_t num_x, size_t num_y);

	void eval_full(size_t num_x, size_t num_y, size_t num_z);
	void eval_full(size_t port_num, size_t num_x, size_t num_y, size_t num_z);

	void eval_xslice(size_t port_num, size_t num_x, size_t num_y, double x);


private:
	box bbox;

	std::vector<node> nodes;
	std::vector<tet> volume_elems;

	ports sim_ports;
	std::vector<Eigen::VectorXd> port_eigen_wave_numbers;
	std::vector<Eigen::MatrixXd> port_eigen_vectors;
	std::vector<std::map<std::pair<size_t, size_t>, size_t>> port_dof_maps;

	std::vector<Eigen::VectorXcd> full_solutions;
	std::map<std::pair<size_t, size_t>, size_t> full_dof_map;
};