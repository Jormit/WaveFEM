#include <Eigen/dense>
#include <complex>

namespace func {
	std::complex<double> free_space_greens_function(Eigen::Vector3d r, Eigen::Vector3d r_, double wavenumber);
	Eigen::Vector3cd free_space_greens_function_grad(Eigen::Vector3d r, Eigen::Vector3d r_, double wavenumber);
}