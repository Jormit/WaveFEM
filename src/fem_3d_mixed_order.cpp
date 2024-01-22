#include "fem.h"

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