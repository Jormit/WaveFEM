#include "pml.h"

material generate_pml(int type, double alpha, double beta)
{
	Eigen::Matrix3cd mat;
	mat.setZero();
	if (type == PML_X)
	{
		mat <<
			std::complex{ alpha, -beta } / (alpha * alpha + beta * beta), 0, 0,
			0, std::complex{ alpha, -beta }, 0,
			0, 0, std::complex{ alpha, -beta };
	}

	else if (type == PML_Y)
	{
		mat <<
			std::complex{ alpha, -beta }, 0, 0,
			0, std::complex{ alpha, -beta } / (alpha * alpha + beta * beta), 0,
			0, 0, std::complex{ alpha, -beta };
	}

	else if (type == PML_X)
	{
		mat <<
			std::complex{ alpha, -beta }, 0, 0,
			0, std::complex{ alpha, -beta }, 0,
			0, 0, std::complex{ alpha, -beta } / (alpha * alpha + beta * beta);
	}

	return { mat, mat };
}
