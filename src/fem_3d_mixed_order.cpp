#include "fem.h"
#include "quad.h"

Eigen::Matrix<double, 20, 3> fem::_3d::mixed_order::basis(const Eigen::Vector4d & lambda, const Eigen::Matrix<double, 4, 3>&nabla_lambda)
{
	Eigen::Matrix<double, 20, 3> func;

	// CT/LN Edge Functions 
	func.row(0)  = lambda(0) * nabla_lambda.row(1) - lambda(1) * nabla_lambda.row(0);
	func.row(1)  = lambda(0) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(0);
	func.row(2)  = lambda(0) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(0);
	func.row(3)  = lambda(1) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(1);
	func.row(4)  = lambda(1) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(1);
	func.row(5)  = lambda(2) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(2);

	// LT/LN Edge Functions
	func.row(6)  = lambda(0) * nabla_lambda.row(1) + lambda(1) * nabla_lambda.row(0);
	func.row(7)  = lambda(0) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(0);
	func.row(8)  = lambda(0) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(0);
	func.row(9)  = lambda(1) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(1);
	func.row(10) = lambda(1) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(1);
	func.row(11) = lambda(2) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(2);

	// LT/QN Face Functions (consecutive functions belong to same face).
	func.row(12) = lambda(1) * lambda(2) * nabla_lambda.row(0) + lambda(0) * lambda(2) * nabla_lambda.row(1) - 2 * lambda(0) * lambda(1) * nabla_lambda.row(2);
	func.row(13) = lambda(2) * lambda(0) * nabla_lambda.row(1) + lambda(1) * lambda(0) * nabla_lambda.row(2) - 2 * lambda(1) * lambda(2) * nabla_lambda.row(0);
	func.row(14) = lambda(1) * lambda(3) * nabla_lambda.row(0) + lambda(0) * lambda(3) * nabla_lambda.row(1) - 2 * lambda(0) * lambda(1) * nabla_lambda.row(3);
	func.row(15) = lambda(3) * lambda(0) * nabla_lambda.row(1) + lambda(1) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(1) * lambda(3) * nabla_lambda.row(0);
	func.row(16) = lambda(2) * lambda(3) * nabla_lambda.row(0) + lambda(0) * lambda(3) * nabla_lambda.row(2) - 2 * lambda(0) * lambda(2) * nabla_lambda.row(3);
	func.row(17) = lambda(3) * lambda(0) * nabla_lambda.row(2) + lambda(2) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(2) * lambda(3) * nabla_lambda.row(0);
	func.row(16) = lambda(2) * lambda(3) * nabla_lambda.row(1) + lambda(1) * lambda(3) * nabla_lambda.row(2) - 2 * lambda(1) * lambda(2) * nabla_lambda.row(3);
	func.row(17) = lambda(3) * lambda(1) * nabla_lambda.row(2) + lambda(2) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(2) * lambda(3) * nabla_lambda.row(1);

	return func;
}

Eigen::Matrix<double, 20, 3> fem::_3d::mixed_order::basis_curl(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda)
{
	Eigen::Matrix<double, 20, 3> func;

	// CT/LN Edge Functions 
	func.row(0)  = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(1));
	func.row(1)  = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(2));
	func.row(2)  = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(3));
	func.row(3)  = 2 * nabla_lambda.row(1).cross(nabla_lambda.row(2));
	func.row(4)  = 2 * nabla_lambda.row(1).cross(nabla_lambda.row(3));
	func.row(5)  = 2 * nabla_lambda.row(2).cross(nabla_lambda.row(3));

	// LT/LN Edge Functions
	func.row(6)  = Eigen::Vector3d::Zero();
	func.row(7)  = Eigen::Vector3d::Zero();
	func.row(8)  = Eigen::Vector3d::Zero();
	func.row(9)  = Eigen::Vector3d::Zero();
	func.row(10) = Eigen::Vector3d::Zero();
	func.row(11) = Eigen::Vector3d::Zero();

	// LT/QN Face Functions (consecutive functions belong to same face).
	func.row(12) = - 3 * lambda(1) * nabla_lambda.row(0).cross(nabla_lambda.row(2)) - 3 * lambda(0) * nabla_lambda.row(1).cross(nabla_lambda.row(2));
	func.row(13) = - 3 * lambda(2) * nabla_lambda.row(1).cross(nabla_lambda.row(0)) - 3 * lambda(1) * nabla_lambda.row(2).cross(nabla_lambda.row(0));
	func.row(14) = - 3 * lambda(1) * nabla_lambda.row(0).cross(nabla_lambda.row(3)) - 3 * lambda(0) * nabla_lambda.row(1).cross(nabla_lambda.row(3));
	func.row(15) = - 3 * lambda(3) * nabla_lambda.row(1).cross(nabla_lambda.row(0)) - 3 * lambda(1) * nabla_lambda.row(3).cross(nabla_lambda.row(0));
	func.row(16) = - 3 * lambda(2) * nabla_lambda.row(0).cross(nabla_lambda.row(3)) - 3 * lambda(0) * nabla_lambda.row(2).cross(nabla_lambda.row(3));
	func.row(17) = - 3 * lambda(3) * nabla_lambda.row(2).cross(nabla_lambda.row(0)) - 3 * lambda(2) * nabla_lambda.row(3).cross(nabla_lambda.row(0));
	func.row(18) = - 3 * lambda(2) * nabla_lambda.row(1).cross(nabla_lambda.row(3)) - 3 * lambda(1) * nabla_lambda.row(2).cross(nabla_lambda.row(3));
	func.row(19) = - 3 * lambda(3) * nabla_lambda.row(2).cross(nabla_lambda.row(1)) - 3 * lambda(2) * nabla_lambda.row(3).cross(nabla_lambda.row(1));

	return func;
}

std::pair<Eigen::Matrix<double, 20, 20>, Eigen::Matrix<double, 20, 20>>
fem::_3d::mixed_order::S_T(const Eigen::Matrix<double, 4, 3>& coords)
{
	Eigen::Matrix<double, 4, 4> simplex_coeff = fem::_3d::simplex_coefficients(coords);
	Eigen::Matrix<double, 4, 3> nabla_lambda = fem::_3d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<double, 20, 20> S = Eigen::Matrix<double, 20, 20>::Zero();
	Eigen::Matrix<double, 20, 20> T = Eigen::Matrix<double, 20, 20>::Zero();
	for (int p = 0; p < 11; p++)
	{
		Eigen::Vector4d lambda;
		lambda << quad::volume::gauss_11_point[p][1], quad::volume::gauss_11_point[p][2], quad::volume::gauss_11_point[p][3], quad::volume::gauss_11_point[p][4];
		auto w = quad::volume::gauss_11_point[p][0];

		auto basis_curl = fem::_3d::mixed_order::basis_curl(lambda, nabla_lambda);
		auto basis = fem::_3d::mixed_order::basis(lambda, nabla_lambda);

		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				S(i, j) += S(i, j) + w * basis_curl(i) * basis_curl(j);
				T(i, j) += T(i, j) + w * basis.row(i).dot(basis.row(j));
			}
		}
	}

	auto volume = fem::_3d::volume(coords);
	return { volume * S, volume * T };
}

Eigen::Matrix<double, 8, 8>	fem::_3d::mixed_order::B(const Eigen::Matrix<double, 3, 2>& coords, double gamma)
{
	Eigen::Matrix<double, 3, 3> simplex_coeff = fem::_2d::simplex_coefficients(coords);
	Eigen::Matrix<double, 3, 2> nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);
	Eigen::Matrix<double, 8, 8> B = Eigen::Matrix<double, 8, 8>::Zero();

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
				B(i, j) += B(i, j) + w * basis.row(i).dot(basis.row(j));
			}
		}
	}

	auto area = fem::_2d::area(coords);
	return B * area;
}