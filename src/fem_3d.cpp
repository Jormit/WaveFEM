#include "fem.h"

Eigen::Matrix<double, 4, 4> fem::_3d::simplex_coefficients(const Eigen::Matrix<double, 4, 3>& coords)
{
	Eigen::Matrix<double, 4, 4> coord_matrix;
	coord_matrix.row(0) = coords.col(0);
	coord_matrix.row(1) = coords.col(1);
	coord_matrix.row(2) = coords.col(2);
	coord_matrix.row(3).setOnes();
	return coord_matrix.inverse();
}

Eigen::Vector4d fem::_3d::lambda(const Eigen::Vector3d& coords, const Eigen::Matrix<double, 4, 4>& simplex_coeff)
{
	Eigen::Vector4d x(coords(0), coords(1), coords(2), 1);
	return simplex_coeff * x;
}

Eigen::Matrix<double, 4, 3> fem::_3d::nabla_lambda(const Eigen::Matrix<double, 4, 4>& simplex_coeff)
{
	return simplex_coeff(Eigen::seq(0, 3), Eigen::seq(0, 2));
}

double fem::_3d::volume(const Eigen::Matrix<double, 4, 3>& coords)
{
	Eigen::Matrix<double, 4, 4> coord_matrix;
	coord_matrix.col(0).setOnes();
	coord_matrix.col(1) = coords.col(0);
	coord_matrix.col(2) = coords.col(1);
	coord_matrix.col(3) = coords.col(2);

	return std::abs(coord_matrix.determinant()) / 6;
}