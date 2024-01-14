#include <functional>
#include "geometry.h"
#include <Eigen/dense>
#include <Eigen/sparse>
#include <array>

namespace fem
{

	void solve_eigenproblem(Eigen::SparseMatrix<double> S, Eigen::SparseMatrix<double> T);
	//void solve_eigenproblem(Eigen::MatrixXd S, Eigen::MatrixXd T);

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

			// Maps edge/face number and identifier to global dof index. (Excludes boundary dofs).
			std::map<std::pair<size_t, size_t>, size_t> dof_map(const std::vector<node>& nodes, const std::vector<tri>& elems);

			// Maps elem and dof_num to pair that can be used to lookup dof_map.
			std::pair<size_t, size_t> global_dof_pair(tri elem, size_t dof_num);

			// Assemble the global matrix.
			std::pair<Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>> assemble_S_T(const std::vector<node>& nodes, const std::vector<tri>& elems, std::map<std::pair<size_t, size_t>, size_t> dof_map);
		}
	}

	namespace _3d
	{
		namespace mixed_order
		{

		}
	}



}