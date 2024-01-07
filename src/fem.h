#include <functional>
#include "geometry.h"
#include "../include/Eigen/dense"
#include <array>

namespace fem
{
	namespace _2d
	{
		Eigen::Matrix<double, 3, 3> simplex_coefficients(Eigen::Matrix<double, 3, 2>& coords);
		Eigen::Vector3d simplex_coordinates(Eigen::Vector2d& coords, Eigen::Matrix<double, 3, 3>& simplex_coeff);
		namespace mixed_order 
		{
			Eigen::Matrix<double, 8, 2> basis(Eigen::Vector3d& lambda, Eigen::Matrix<double, 3, 2>& nabla_lambda);
			Eigen::Matrix<double, 8, 1> basis_curl(Eigen::Vector3d& lambda, Eigen::Matrix<double, 3, 2>& nabla_lambda);

			Eigen::Matrix<double, 8, 8> S();
			Eigen::Matrix<double, 8, 8> T();
		}
	}

	namespace _3d
	{
		namespace mixed_order 
		{

		}
	}


	
}