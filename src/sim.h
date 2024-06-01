#pragma once
#include <vector>
#include <Eigen/Dense>
#include <map>
#include <unordered_set>
#include <string>

#include "ports.h"
#include "geometry.h"
#include "material.h"
#include "config.h"
#include "fem.h"

struct sim
{
	sim(geo::box bbox, double wavenumber, std::vector<material> materials, std::vector<geo::node> nodes, std::vector<geo::tet> volume_elems,
		std::unordered_map<size_t, int> boundary_edge_map, std::unordered_map<size_t, int> boundary_face_map, ports ports);

	static sim create(sim_config config, std::string data_path);

	void solve_ports();
	void solve_full();
	void generate_outputs(std::string directory, sim_config config);

	// Input
	geo::box bbox;
	double wavenumber;
	std::vector<material> materials;
	std::unordered_map<size_t, size_t> volume_material_map;
	std::vector<geo::node> nodes;
	std::vector<geo::tet> volume_elems;
	std::unordered_map<size_t, int> boundary_edge_map;
	std::unordered_map<size_t, int> boundary_face_map;
	ports sim_ports;

	// Port Solution
	std::vector<Eigen::VectorXd> port_eigen_wave_numbers;
	std::vector<Eigen::MatrixXd> port_eigen_vectors;
	std::vector<fem::dof_map> port_dof_maps;

	// Full Solution
	std::vector<Eigen::VectorXcd> full_solutions;
	fem::dof_map full_dof_map;
};