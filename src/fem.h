#pragma once
#include <functional>
#include "geometry.h"
#include <Eigen/dense>
#include <Eigen/sparse>
#include <array>

namespace fem
{

    std::pair<Eigen::VectorXd, Eigen::MatrixXd> solve_eigenproblem(const Eigen::SparseMatrix<double>& S, const Eigen::SparseMatrix<double>& T);

	namespace _2d
	{
		Eigen::Matrix<double, 3, 3> simplex_coefficients(const Eigen::Matrix<double, 3, 2>& coords);
		Eigen::Vector3d lambda(const Eigen::Vector2d& coords, const Eigen::Matrix<double, 3, 3>& simplex_coeff);
		Eigen::Matrix<double, 3, 2> nabla_lambda(const Eigen::Matrix<double, 3, 3>& simplex_coeff);
		double area(const Eigen::Matrix<double, 3, 2>& coords);

		namespace mixed_order
		{
			Eigen::Matrix<double, 8, 2> basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);
			Eigen::Matrix<double, 8, 1> basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			std::pair<Eigen::Matrix<double, 8, 8>, Eigen::Matrix<double, 8, 8>>
				S_T(const Eigen::Matrix<double, 3, 2>& coords);

			std::map<std::pair<size_t, size_t>, size_t> dof_map(const std::vector<node>& nodes, const std::vector<tri>& elems);
			std::pair<size_t, size_t> global_dof_pair(const tri& elem, const size_t& dof_num);

			std::pair<Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>>
				assemble_S_T(const std::vector<node>& nodes, const std::vector<tri>& elems,
					const std::map<std::pair<size_t, size_t>, size_t>& dof_map);

			Eigen::Vector2d eval_elem(const std::vector<node>& nodes, const tri& e, const point_2d& eval_point,
				const std::map<std::pair<size_t, size_t>, size_t>& dof_map, const Eigen::VectorXd& solution);
		}
	}

	namespace _3d
	{
		Eigen::Matrix<double, 4, 4> simplex_coefficients(const Eigen::Matrix<double, 4, 3>& coords);
		Eigen::Vector4d lambda(const Eigen::Vector3d& coords, const Eigen::Matrix<double, 4, 4>& simplex_coeff);
		Eigen::Matrix<double, 4, 3> nabla_lambda(const Eigen::Matrix<double, 4, 4>& simplex_coeff);
		double volume(const Eigen::Matrix<double, 4, 3>& coords);

		namespace mixed_order
		{
			Eigen::Matrix<double, 20, 3> basis(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda);
			Eigen::Matrix<double, 20, 3> basis_curl(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda);

			std::pair<Eigen::Matrix<double, 20, 20>, Eigen::Matrix<double, 20, 20>>
				S_T(const Eigen::Matrix<double, 4, 3>& coords);


		}
	}



}