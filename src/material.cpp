#include <iostream>

#include "material.h"

material mat::vacuum()
{
	return { 1, 1 };
}

std::vector<material> mat::generate_base_set()
{
	auto free_space = vacuum();	

	std::complex<double> s_factor{ 1, 5 };

	auto pml_x = pml(s_factor, 1, 1);
	auto pml_y = pml(1, s_factor, 1);
	auto pml_z = pml(1, 1, s_factor);

	auto pml_xy = pml(s_factor, s_factor, 1);
	auto pml_xz = pml(s_factor, 1, s_factor);
	auto pml_yz = pml(1, s_factor, s_factor);
	
	auto pml_xyz = pml(s_factor, s_factor, s_factor);

	return { free_space, pml_x, pml_y, pml_z, pml_xy, pml_xz, pml_yz, pml_xyz };
}

std::vector<material> mat::load_user_materials(std::vector<material_config> materials)
{
	std::vector<material> user_materials;
	for (const auto & mat : materials)
	{
		if (!mat.PEC)
		{
			user_materials.push_back(material(mat.ep, mat.mu));
		}
	}
	return user_materials;
}

void mat::label_elems(std::vector<tet>& elems, size_t material)
{
	for (int i = 0; i < elems.size(); i++)
	{
		elems[i].material_id = material;
	}
}

void mat::label_elems(std::vector<tri>& elems, size_t material)
{
	for (int i = 0; i < elems.size(); i++)
	{
		elems[i].material_id = material;
	}
}

material mat::pml(std::complex<double> sx, std::complex<double> sy, std::complex<double> sz)
{
	Eigen::Matrix3cd pml;
	pml <<
		sy * sz / sx, 0, 0,
		0, sz * sx / sy, 0,
		0, 0, sx * sy / sz;
	return { pml , pml };
}

material::material(std::complex<double> permittivity, std::complex<double> permeability)
{
	this->permittivity = permittivity * Eigen::Matrix3cd::Identity();
	this->permeability = permeability * Eigen::Matrix3cd::Identity();
	isotropic = true;
}

material::material(Eigen::Matrix3cd permittivity, Eigen::Matrix3cd permeability) :
	permittivity(permittivity), permeability(permeability), isotropic(false)
{}