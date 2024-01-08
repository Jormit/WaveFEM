#include "fem.h"
//#include "quad.h"
#include <iostream>

using namespace fem::_2d;

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

Eigen::Matrix<double, 8, 2> mixed_order::basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
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

Eigen::Matrix<double, 8, 1> mixed_order::basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda) {
	Eigen::Matrix<double, 8, 1> func;
	Eigen::Matrix<double, 3, 3> nabla_matrix;
	nabla_matrix.col(0) = nabla_lambda.col(0);
	nabla_matrix.col(1) = nabla_lambda.col(1);
	nabla_matrix.col(2).setZero();

	func(0) = 2 * nabla_matrix.row(0).cross(nabla_matrix.row(1))(2);
	func(1) = 2 * nabla_matrix.row(0).cross(nabla_matrix.row(2))(2);
	func(2) = 2 * nabla_matrix.row(1).cross(nabla_matrix.row(2))(2);
	func(3) = 0;
	func(4) = 0;
	func(5) = 0;
	func(6) = -3 * lambda(1) * nabla_matrix.row(0).cross(nabla_matrix.row(2))(2) - 3 * lambda(0) * nabla_matrix.row(1).cross(nabla_matrix.row(2))(2);
	func(7) = -3 * lambda(2) * nabla_matrix.row(1).cross(nabla_matrix.row(0))(2) - 3 * lambda(1) * nabla_matrix.row(2).cross(nabla_matrix.row(0))(2);

	return func;
}

Eigen::Matrix<double, 8, 8> mixed_order::S(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	
	return {};
}

Eigen::Matrix<double, 8, 8> mixed_order::T(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	return {};
}