#include "fem.h"
#include "quad.h"

#include <iostream>

using namespace fem::_2d;

Eigen::Matrix<double, 3, 3> fem::_2d::simplex_coefficients(Eigen::Matrix<double, 3, 2>& coords)
{
	Eigen::Matrix<double, 3, 3> coord_matrix;
	coord_matrix.row(0) = coords.col(0);
	coord_matrix.row(1) = coords.col(1);
	coord_matrix.row(2).setOnes();
	return coord_matrix.inverse();
}

Eigen::Vector3d fem::_2d::simplex_coordinates(Eigen::Vector2d& coords, Eigen::Matrix<double, 3, 3>& simplex_coeff)
{
	Eigen::Vector3d x(coords(0), coords(1), 1);
	return simplex_coeff * x;
}

Eigen::Matrix<double, 8, 2> mixed_order::basis(Eigen::Vector3d& lambda, Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	Eigen::Matrix<double, 8, 2> func;

	func.row(0) = lambda(0) * nabla_lambda.row(1) - lambda(1) * nabla_lambda.row(0);
	func.row(1) = lambda(0) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(0);
	func.row(2) = lambda(1) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(1);
	func.row(3) = lambda(0) * nabla_lambda.row(1) + lambda(1) * nabla_lambda.row(0);
	func.row(4) = lambda(0) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(0);
	func.row(5) = lambda(1) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(1);
	func.row(6) = (lambda(1) * lambda(2) * nabla_lambda.row(0)) + (lambda(0) * lambda(2) * nabla_lambda.row(1)) - 2 * (lambda(0) * lambda(1) * nabla_lambda.row(2));
	func.row(7) = (lambda(2) * lambda(0) * nabla_lambda.row(1)) + (lambda(1) * lambda(0) * nabla_lambda.row(2)) - 2 * (lambda(1) * lambda(2) * nabla_lambda.row(0));
	
	return func;
}
Eigen::Matrix<double, 8, 1> mixed_order::basis_curl(Eigen::Vector3d& lambda, Eigen::Matrix<double, 3, 2>& nabla_lambda) {
	Eigen::Matrix<double, 8, 1> func;
	return func;
}
