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
			Eigen::Matrix<double, 8, 2> basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);
			Eigen::Matrix<double, 8, 1> basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda);

			Eigen::Matrix<double, 8, 8> S(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda);
			Eigen::Matrix<double, 8, 8> T(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda);
		}
	}

	namespace _3d
	{
		namespace mixed_order
		{

		}
	}



}