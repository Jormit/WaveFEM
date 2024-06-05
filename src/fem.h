#pragma once
#include <functional>
#include <Eigen/dense>
#include <Eigen/sparse>
#include <array>
#include <complex>
#include <unordered_set>

#include "geometry.h"
#include "material.h"

namespace fem
{
	std::pair<Eigen::VectorXd, Eigen::MatrixXd> solve_eigenproblem(const Eigen::MatrixXd& S, const Eigen::MatrixXd& T, double min);
	std::pair<Eigen::VectorXd, Eigen::MatrixXd> solve_eigenproblem_iram(const Eigen::SparseMatrix<double>& S, const Eigen::SparseMatrix<double>& T, double guess, int num);

	enum dof_type
	{
		NODE_1, NODE_2,
		EDGE_1, EDGE_2,
		FACE_1, FACE_2
	};

	typedef std::pair<size_t, dof_type> dof_pair;
	typedef std::map<dof_pair, size_t> dof_map;

	namespace _2d
	{
		Eigen::Matrix<double, 3, 3> simplex_coefficients(const Eigen::Matrix<double, 3, 2>& coords);
		Eigen::Vector3d lambda(const Eigen::Vector2d& coords, const Eigen::Matrix<double, 3, 3>& simplex_coeff);
		Eigen::Matrix<double, 3, 2> nabla_lambda(const Eigen::Matrix<double, 3, 3>& simplex_coeff);
		double area(const Eigen::Matrix<double, 3, 2>& coords);

		namespace mixed_order
		{
			Eigen::Matrix<double, 8, 2> vector_basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);
			Eigen::Matrix<double, 8, 1> vector_basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			Eigen::Matrix<double, 6, 1> scalar_basis(const Eigen::Vector3d& lambda);
			Eigen::Matrix<double, 6, 2> scalar_basis_grad(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			std::pair<Eigen::Matrix<double, 14, 14>, Eigen::Matrix<double, 14, 14>>
				A_B(const Eigen::Matrix<double, 3, 2>& coords, material mat, double k0);

			dof_map generate_dof_map(const std::vector<geo::node>& nodes,
				const std::vector<geo::tri>& elems, std::unordered_map<size_t, int> boundary_edge_map);
			dof_pair global_dof_pair(const geo::tri& elem, const size_t& dof_num);

			std::pair<Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>>
				assemble_A_B(const std::vector<geo::node>& nodes, const std::vector<geo::tri>& elems,
					std::vector<material> materials, const dof_map& dof_map, double k0);

			Eigen::Vector2cd eval_elem(const std::vector<geo::node>& nodes, const geo::tri& e,
				const geo::point_2d& eval_point, const dof_map& dof_map, const Eigen::VectorXcd& solution);

			Eigen::Vector2cd eval_elem(const geo::tri& e, const Eigen::Vector3d& lambda,
				const Eigen::Matrix<double, 3, 2>& nabla_lambda, const dof_map& dof_map, const Eigen::VectorXcd& solution);
		}
	}

	namespace _3d
	{
		Eigen::Matrix<double, 4, 4> simplex_coefficients(const Eigen::Matrix<double, 4, 3>& coords);
		Eigen::Vector4d lambda(const Eigen::Vector3d& coords, const Eigen::Matrix<double, 4, 4>& simplex_coeff);
		Eigen::Matrix<double, 4, 3> nabla_lambda(const Eigen::Matrix<double, 4, 4>& simplex_coeff);
		double volume(const Eigen::Matrix<double, 4, 3>& coords);
		Eigen::Vector4d project_2d_to_3d_lambda(Eigen::Vector3d lambda, geo::tri e_2d, geo::tet e_3d);

		namespace mixed_order
		{
			Eigen::Matrix<double, 20, 3> basis(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda);
			Eigen::Matrix<double, 20, 3> basis_curl(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda);

			std::pair<Eigen::Matrix<std::complex<double>, 20, 20>, Eigen::Matrix<std::complex<double>, 20, 20>>
				S_T(const Eigen::Matrix<double, 4, 3>& coords, material mat);

			Eigen::Matrix<double, 8, 8> B(const Eigen::Matrix<double, 3, 2>& coords);

			Eigen::Matrix<std::complex<double>, 8, 1> b(const geo::tri& e, const Eigen::Matrix<double, 3, 2>& coords,
				const dof_map& dof_map, const Eigen::VectorXd& solution);

			dof_map generate_dof_map(const std::vector<geo::tet>& elems,
				std::unordered_map<size_t, int> boundary_edge_map, std::unordered_map<size_t, int> boundary_face_map);
			dof_pair global_dof_pair(const geo::tet& elem, const size_t& dof_num);

			Eigen::SparseMatrix<std::complex<double>> assemble_A(const std::vector<geo::node>& nodes, const std::vector<geo::tet>& elems,
				std::vector<material> materials, const std::vector<std::vector<geo::tri>>& boundary_elems, const dof_map& dof_map,
				std::complex<double> ki, std::vector<std::complex<double>> boundary_k);

			Eigen::VectorXcd assemble_b(const std::vector<geo::node>& nodes, const std::vector<geo::tri>& surface_elems,
				const dof_map& solution_dof_map, const dof_map& excitation_dof_map,
				const Eigen::VectorXd& excitation, std::complex<double> ki);

			Eigen::Vector3cd eval_elem(const std::vector<geo::node>& nodes, const geo::tet& e, const geo::point_3d& eval_point,
				const dof_map& dof_map, const Eigen::VectorXcd& solution);

			Eigen::Vector3cd eval_elem(const geo::tet& e, const Eigen::Vector4d& lambda,
				const Eigen::Matrix<double, 4, 3>& nabla_lambda, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution);

			Eigen::Vector3cd eval_elem_curl(const std::vector<geo::node>& nodes, const geo::tet& e, const geo::point_3d& eval_point,
				const dof_map& dof_map, const Eigen::VectorXcd& solution);

			Eigen::Vector3cd eval_elem_curl(const geo::tet& e, const Eigen::Vector4d& lambda,
				const Eigen::Matrix<double, 4, 3>& nabla_lambda, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution);

		}
	}
}