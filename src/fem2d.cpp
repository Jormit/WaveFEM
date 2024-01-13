#include "fem.h"
#include "quad.h"
#include <iostream>

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

Eigen::Matrix<double, 8, 2> fem::_2d::mixed_order::basis(const Eigen::Vector3d & lambda, const Eigen::Matrix<double, 3, 2>&nabla_lambda)
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

Eigen::Matrix<double, 8, 1> fem::_2d::mixed_order::basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda) {
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

std::pair<Eigen::Matrix<double, 8, 8>, Eigen::Matrix<double, 8, 8>> 
fem::_2d::mixed_order::S_T(const Eigen::Matrix<double, 3, 3>& simplex_coeff, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	Eigen::Matrix<double, 8, 8> S = Eigen::Matrix<double, 8, 8>::Zero();
	Eigen::Matrix<double, 8, 8> T = Eigen::Matrix<double, 8, 8>::Zero();
	for (int p = 0; p < 6; p++)
	{
		Eigen::Vector3d lambda;
		lambda << quad::surface::gauss_6_point[p][1], quad::surface::gauss_6_point[p][2], quad::surface::gauss_6_point[p][3];
		auto w = quad::surface::gauss_6_point[p][0];

		auto basis_curl = fem::_2d::mixed_order::basis_curl(lambda, nabla_lambda);
		auto basis = fem::_2d::mixed_order::basis(lambda, nabla_lambda);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				S(i, j) += S(i, j) + w * basis_curl(i) * basis_curl(j);
				T(i, j) += T(i, j) + w * basis_curl(i) * basis_curl(j);
			}
		}
	}
	return { S, T };
}

std::pair<Eigen::MatrixXf, Eigen::MatrixXf> fem::_2d::mixed_order::assemble_S_T(const std::vector<node>& nodes, const std::vector<tri>& elems)
{
	for (auto e : elems)
	{
		
	}
	return {};
}