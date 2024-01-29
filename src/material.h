#include <Eigen/dense>

struct material
{
	Eigen::MatrixXcd permeability;
	Eigen::MatrixXcd permittivity;
};