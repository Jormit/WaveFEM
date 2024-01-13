#include "fem.h"

Eigen::Matrix<double, 3, 3> fem::_2d::simplex_coefficients(const Eigen::Matrix<double, 3, 2>& coords)
{
	Eigen::Matrix<double, 3, 3> coord_matrix;
	coord_matrix.row(0) = coords.col(0);
	coord_matrix.row(1) = coords.col(1);
	coord_matrix.row(2).setOnes();
	return coord_matrix.inverse();
}

Eigen::Vector3d fem::_2d::lambda(const Eigen::Vector2d& coords, const Eigen::Matrix<double, 3, 3>& simplex_coeff)
{
	Eigen::Vector3d x(coords(0), coords(1), 1);
	return simplex_coeff * x;
}

Eigen::Matrix<double, 3, 2> fem::_2d::nabla_lambda(const Eigen::Matrix<double, 3, 3>& simplex_coeff)
{
	return simplex_coeff(Eigen::seq(0, 2), Eigen::seq(0, 1));
}