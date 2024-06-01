#include "pml.h"
#include "helpers.h"
#include "material.h"

pml_boundary pml::create(double pml_box_padding)
{
	auto free_space_bounds = mesher_interface::get_bounding_box();
	auto pml_bounds = free_space_bounds;
	pml_bounds.add_padding(pml_box_padding);

	geo::box x_up(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmax);
	geo::box x_down(pml_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax);

	geo::box y_up(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymax, free_space_bounds.zmax);
	geo::box y_down(free_space_bounds.xmin, pml_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax);

	geo::box z_up(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		free_space_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmax);
	geo::box z_down(free_space_bounds.xmin, free_space_bounds.ymin, pml_bounds.zmin,
		free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin);

	geo::box xy_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymin, free_space_bounds.zmax);
	geo::box xy_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymin, free_space_bounds.zmax);
	geo::box xy_3(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymax, free_space_bounds.zmax);
	geo::box xy_4(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymax, free_space_bounds.zmax);

	geo::box xz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, free_space_bounds.ymax, pml_bounds.zmin);
	geo::box xz_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmin);
	geo::box xz_3(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmin, free_space_bounds.ymax, pml_bounds.zmax);
	geo::box xz_4(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmax);

	geo::box yz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymin, pml_bounds.zmin);
	geo::box yz_2(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymax, pml_bounds.zmin);
	geo::box yz_3(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		free_space_bounds.xmax, pml_bounds.ymin, pml_bounds.zmax);
	geo::box yz_4(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax,
		free_space_bounds.xmax, pml_bounds.ymax, pml_bounds.zmax);

	geo::box xyz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymin, pml_bounds.zmin);
	geo::box xyz_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymin, pml_bounds.zmin);
	geo::box xyz_3(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymax, pml_bounds.zmin);
	geo::box xyz_4(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymax, pml_bounds.zmin);
	geo::box xyz_5(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmin, pml_bounds.ymin, pml_bounds.zmax);
	geo::box xyz_6(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmax, pml_bounds.ymin, pml_bounds.zmax);
	geo::box xyz_7(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax,
		pml_bounds.xmin, pml_bounds.ymax, pml_bounds.zmax);
	geo::box xyz_8(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmax,
		pml_bounds.xmax, pml_bounds.ymax, pml_bounds.zmax);

	std::array<int, 2> x_ids;
	std::array<int, 2> y_ids;
	std::array<int, 2> z_ids;
	x_ids[0] = mesher_interface::add_box(x_up);
	x_ids[1] = mesher_interface::add_box(x_down);

	y_ids[0] = mesher_interface::add_box(y_up);
	y_ids[1] = mesher_interface::add_box(y_down);

	z_ids[0] = mesher_interface::add_box(z_up);
	z_ids[1] = mesher_interface::add_box(z_down);

	std::array<int, 4> xy_ids;
	std::array<int, 4> xz_ids;
	std::array<int, 4> yz_ids;
	xy_ids[0] = mesher_interface::add_box(xy_1);
	xy_ids[1] = mesher_interface::add_box(xy_2);
	xy_ids[2] = mesher_interface::add_box(xy_3);
	xy_ids[3] = mesher_interface::add_box(xy_4);

	xz_ids[0] = mesher_interface::add_box(xz_1);
	xz_ids[1] = mesher_interface::add_box(xz_2);
	xz_ids[2] = mesher_interface::add_box(xz_3);
	xz_ids[3] = mesher_interface::add_box(xz_4);

	yz_ids[0] = mesher_interface::add_box(yz_1);
	yz_ids[1] = mesher_interface::add_box(yz_2);
	yz_ids[2] = mesher_interface::add_box(yz_3);
	yz_ids[3] = mesher_interface::add_box(yz_4);

	std::array<int, 8> xyz_ids;
	xyz_ids[0] = mesher_interface::add_box(xyz_1);
	xyz_ids[1] = mesher_interface::add_box(xyz_2);
	xyz_ids[2] = mesher_interface::add_box(xyz_3);
	xyz_ids[3] = mesher_interface::add_box(xyz_4);
	xyz_ids[4] = mesher_interface::add_box(xyz_5);
	xyz_ids[5] = mesher_interface::add_box(xyz_6);
	xyz_ids[6] = mesher_interface::add_box(xyz_7);
	xyz_ids[7] = mesher_interface::add_box(xyz_8);

	gmsh::model::occ::synchronize();

	return { x_ids, y_ids, z_ids, xy_ids, xz_ids, yz_ids, xyz_ids };
}

void pml::label_elements(pml_boundary pml_volumes, std::vector<geo::tet>& elems)
{
	mesher_interface::label_elems_in_volume(
		{ 
			pml_volumes.x_ids[0],
			pml_volumes.x_ids[1]
		},
		mat::PML_X, elems);

	mesher_interface::label_elems_in_volume(
		{
			pml_volumes.y_ids[0],
			pml_volumes.y_ids[1]
		},
		mat::PML_Y, elems);

	mesher_interface::label_elems_in_volume(
		{
			pml_volumes.z_ids[0],
			pml_volumes.z_ids[1]
		},
		mat::PML_Z, elems);

	mesher_interface::label_elems_in_volume(
		{ 
			pml_volumes.xy_ids[0],
			pml_volumes.xy_ids[1],
			pml_volumes.xy_ids[2],
			pml_volumes.xy_ids[3] 
		},
		mat::PML_XY, elems);

	mesher_interface::label_elems_in_volume(
		{
			pml_volumes.xz_ids[0],
			pml_volumes.xz_ids[1],
			pml_volumes.xz_ids[2],
			pml_volumes.xz_ids[3]
		},
		mat::PML_XZ, elems);

	mesher_interface::label_elems_in_volume(
		{
			pml_volumes.yz_ids[0],
			pml_volumes.yz_ids[1],
			pml_volumes.yz_ids[2],
			pml_volumes.yz_ids[3]
		},
		mat::PML_YZ, elems);

	mesher_interface::label_elems_in_volume(
		{
			pml_volumes.xyz_ids[0],
			pml_volumes.xyz_ids[1],
			pml_volumes.xyz_ids[2],
			pml_volumes.xyz_ids[3],
			pml_volumes.xyz_ids[4],
			pml_volumes.xyz_ids[5],
			pml_volumes.xyz_ids[6],
			pml_volumes.xyz_ids[7]
		},
		mat::PML_XYZ, elems);
}
