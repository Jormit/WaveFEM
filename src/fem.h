#include <functional>
#include "geometry.h"
#include "../include/Eigen/dense"
#include <array>

namespace fem
{

	namespace _2d
	{
		Eigen::Matrix<double, 3, 3> simplex_coefficients(const Eigen::Matrix<double, 3, 2>& coords);
		Eigen::Vector3d lambda(const Eigen::Vector2d& coords, const Eigen::Matrix<double, 3, 3>& simplex_coeff);
		Eigen::Matrix<double, 3, 2> nabla_lambda(const Eigen::Matrix<double, 3, 3>& simplex_coeff);

		namespace mixed_order
		{
			// Evaluates all basis functions at given simplex coordinates and nabla lambda.
			Eigen::Matrix<double, 8, 2> basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			// Evaluates the curl of all basis functions at given simplex coordinates and nabla lambda.
			Eigen::Matrix<double, 8, 1> basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			// Evaluates local S and T matrices for given nabla lambda.
			std::pair<Eigen::Matrix<double, 8, 8>, Eigen::Matrix<double, 8, 8>>
				S_T(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			// Maps elem number and local dof index to global dof index. 
			std::unordered_map<std::pair<int, int>, int> dof_map(const std::vector<node>& nodes, const std::vector<tri>& elems);

			std::pair<Eigen::MatrixXf, Eigen::MatrixXf> assemble_S_T(const std::vector<node>& nodes, const std::vector<tri>& elems);
		}
	}

	namespace _3d
	{
		namespace mixed_order
		{

		}
	}



}