#include "pml.h"
#include "helpers.h"
#include "material.h"

pml_boundary pml::create(point_3d pml_box_padding)
{
	auto free_space_bounds = mesher_interface::get_bounding_box();
	auto pml_bounds = free_space_bounds;
	pml_bounds.add_padding(pml_box_padding);

	box x_up(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmax);
	box x_down(pml_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax);

	box y_up(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymax, free_space_bounds.zmax);
	box y_down(free_space_bounds.xmin, pml_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax);

	box z_up(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		free_space_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmax);
	box z_down(free_space_bounds.xmin, free_space_bounds.ymin, pml_bounds.zmin,
		free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin);

	box xy_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymin, free_space_bounds.zmax);
	box xy_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymin, free_space_bounds.zmax);
	box xy_3(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymax, free_space_bounds.zmax);
	box xy_4(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymax, free_space_bounds.zmax);

	box xz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, free_space_bounds.ymax, pml_bounds.zmin);
	box xz_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmin);
	box xz_3(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmin, free_space_bounds.ymax, pml_bounds.zmax);
	box xz_4(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmax, free_space_bounds.ymax, pml_bounds.zmax);

	box yz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymin, pml_bounds.zmin);
	box yz_2(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		free_space_bounds.xmax, pml_bounds.ymax, pml_bounds.zmin);
	box yz_3(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		free_space_bounds.xmax, pml_bounds.ymin, pml_bounds.zmax);
	box yz_4(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax,
		free_space_bounds.xmax, pml_bounds.ymax, pml_bounds.zmax);

	box xyz_1(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymin, pml_bounds.zmin);
	box xyz_2(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymin, pml_bounds.zmin);
	box xyz_3(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmin, pml_bounds.ymax, pml_bounds.zmin);
	box xyz_4(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmin,
		pml_bounds.xmax, pml_bounds.ymax, pml_bounds.zmin);
	box xyz_5(free_space_bounds.xmin, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmin, pml_bounds.ymin, pml_bounds.zmax);
	box xyz_6(free_space_bounds.xmax, free_space_bounds.ymin, free_space_bounds.zmax,
		pml_bounds.xmax, pml_bounds.ymin, pml_bounds.zmax);
	box xyz_7(free_space_bounds.xmin, free_space_bounds.ymax, free_space_bounds.zmax,
		pml_bounds.xmin, pml_bounds.ymax, pml_bounds.zmax);
	box xyz_8(free_space_bounds.xmax, free_space_bounds.ymax, free_space_bounds.zmax,
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

std::vector<tet> pml::get_elements(pml_boundary pml_volumes)
{
	std::vector<tet> elements;

	auto x_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.x_ids[0], pml_volumes.x_ids[1] }));
	mat::label_elems(x_elems, mat::PML_X);
	elements.insert(elements.end(), x_elems.begin(), x_elems.end());

	auto y_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.y_ids[0], pml_volumes.y_ids[1] }));
	mat::label_elems(y_elems, mat::PML_Y);
	elements.insert(elements.end(), y_elems.begin(), y_elems.end());

	auto z_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.z_ids[0], pml_volumes.z_ids[1] }));
	mat::label_elems(z_elems, mat::PML_Z);
	elements.insert(elements.end(), z_elems.begin(), z_elems.end());
	
	auto xy_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.xy_ids[0], pml_volumes.xy_ids[1], pml_volumes.xy_ids[2], pml_volumes.xy_ids[3] }));
	mat::label_elems(xy_elems, mat::PML_XY);
	elements.insert(elements.end(), xy_elems.begin(), xy_elems.end());

	auto xz_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.xz_ids[0], pml_volumes.xz_ids[1], pml_volumes.xz_ids[2], pml_volumes.xz_ids[3] }));
	mat::label_elems(xz_elems, mat::PML_XZ);
	elements.insert(elements.end(), xz_elems.begin(), xz_elems.end());

	auto yz_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ pml_volumes.yz_ids[0], pml_volumes.yz_ids[1], pml_volumes.yz_ids[2], pml_volumes.yz_ids[3] }));
	mat::label_elems(yz_elems, mat::PML_YZ);
	elements.insert(elements.end(), yz_elems.begin(), yz_elems.end());

	auto xyz_elems = helpers::flatten_vector(
		mesher_interface::get_volume_elems({ 
			pml_volumes.xyz_ids[0], pml_volumes.xyz_ids[1], pml_volumes.xyz_ids[2], pml_volumes.xyz_ids[3],
			pml_volumes.xyz_ids[4], pml_volumes.xyz_ids[5], pml_volumes.xyz_ids[6], pml_volumes.xyz_ids[7] }));
	mat::label_elems(xyz_elems, mat::PML_XYZ);
	elements.insert(elements.end(), xyz_elems.begin(), xyz_elems.end());

	return elements;
}
