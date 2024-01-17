#pragma once
#include <vector>
#include "geometry.h"
#include <Eigen/Dense>
#include <map>

// This class manages all the elements within a simulation context
class sim
{
public:
	sim(std::vector<node> nodes, std::vector<tet> volume_elems, std::vector<int> port_surface_ids, std::vector<std::vector<tri>> port_elems);
	void solve_ports();

private:
	// Element information
	std::vector<node> nodes;
	std::vector<tet> volume_elems;
	std::vector<int> port_surface_ids;
	std::vector<std::vector<tri>> port_elems;

	// Results
	std::vector<Eigen::VectorXd> port_eigen_wave_numbers;
	std::vector<Eigen::MatrixXd> port_eigen_vectors;
	std::vector<std::map<std::pair<size_t, size_t>, size_t>> port_dof_maps;
};