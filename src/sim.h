#pragma once
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <unordered_set>

#include "ports.h"
#include "geometry.h"
#include "material.h"

class sim
{
public:
	sim(box bbox, std::vector<material> materials, std::vector<node> nodes, std::vector<tet> volume_elems,
		std::unordered_set<size_t> boundary_edges, std::unordered_set<size_t> boundary_faces, ports ports);

	void solve_ports();
	void solve_full(double k);

	friend class post_processor;

private:
	// Input
	box bbox;
	std::vector<material> materials;
	std::vector<node> nodes;
	std::vector<tet> volume_elems;
	std::unordered_set<size_t> boundary_edges;
	std::unordered_set<size_t> boundary_faces;
	ports sim_ports;

	// Port Solution
	std::vector<Eigen::VectorXd> port_eigen_wave_numbers;
	std::vector<Eigen::MatrixXd> port_eigen_vectors;
	std::vector<std::map<std::pair<size_t, size_t>, size_t>> port_dof_maps;

	// Full Solution
	std::vector<Eigen::VectorXcd> full_solutions;
	std::map<std::pair<size_t, size_t>, size_t> full_dof_map;
};