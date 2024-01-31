#include "material.h"

material default_material()
{
	return { Eigen::Matrix3cd::Identity() , Eigen::Matrix3cd::Identity() };
	return { Eigen::Matrix3cd::Identity() , Eigen::Matrix3cd::Identity() };
}

std::vector<material> generate_base_material_set()
{
	auto free_space = default_material();

	double alpha = 1;
	double beta = 2;

	Eigen::Matrix3cd pml_x;
	pml_x <<
		std::complex{ alpha, -beta } / (alpha * alpha + beta * beta), 0, 0,
		0, std::complex{ alpha, -beta }, 0,
		0, 0, std::complex{ alpha, -beta };

	Eigen::Matrix3cd pml_y;
	pml_y <<
		std::complex{ alpha, -beta } / (alpha * alpha + beta * beta), 0, 0,
		0, std::complex{ alpha, -beta }, 0,
		0, 0, std::complex{ alpha, -beta };

	Eigen::Matrix3cd pml_z;
	pml_z <<
		std::complex{ alpha, -beta }, 0, 0,
		0, std::complex{ alpha, -beta }, 0,
		0, 0, std::complex{ alpha, -beta } / (alpha * alpha + beta * beta);

	return { free_space, {pml_x, pml_x}, {pml_y, pml_y}, {pml_z, pml_z} };
}

void label_pml_elements(std::vector<tet>& elems, box non_pml_boundary, const std::vector<node>& nodes)
{
	for (size_t i = 0; i < elems.size(); i++)
	{
		auto center = elems[i].get_center(nodes);
		if (center.x > non_pml_boundary.xmax || center.x < non_pml_boundary.xmin)
		{
			elems[i].material_id = PML_X;
		}
		else if (center.y > non_pml_boundary.ymax || center.y < non_pml_boundary.ymin)
		{
			elems[i].material_id = PML_Y;
		}
		else
		{
			elems[i].material_id = PML_Z;
		}
	}
}