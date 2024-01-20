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
	sim(std::vector<node> nodes, std::vector<tet> volume_elems, ports ports);
	void solve_ports();
	std::vector<Eigen::Vector2d> eval_port(size_t port_num, size_t num_x, size_t num_y);

private:
	// Element information
	std::vector<node> nodes;
	std::vector<tet> volume_elems;

	// Port information
	ports sim_ports;

	// Results
	std::vector<Eigen::VectorXd> port_eigen_wave_numbers;
	std::vector<Eigen::MatrixXd> port_eigen_vectors;
	std::vector<std::map<std::pair<size_t, size_t>, size_t>> port_dof_maps;
};