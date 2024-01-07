#include <functional>
#include "geometry.h"
#include "../include/Eigen/dense"

namespace fem
{
	namespace mixed_order {
		namespace _2d
		{
			Eigen::MatrixXd S();
			Eigen::MatrixXd T();
		}

		namespace _3d
		{
			Eigen::MatrixXd S();
			Eigen::MatrixXd T();
			Eigen::MatrixXd B();
			Eigen::VectorXd b();
		}
	}
}