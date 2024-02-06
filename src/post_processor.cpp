#include <format>
#include <fstream>

#include "post_processor.h"
#include "mesher_interface.h"
#include "result_formatter.h"
#include "fem.h"

post_processor::post_processor(std::unique_ptr<sim> sim_instance) :
	sim_instance(std::move(sim_instance))
{}

void post_processor::eval_port(size_t num_x, size_t num_y)
{
	for (size_t p = 0; p < sim_instance->sim_ports.entity_ids.size(); p++)
	{
		for (size_t m = 0; m < sim_instance->port_eigen_vectors[0].cols(); m++)
		{
			eval_port(p, m, num_x, num_y);
		}
	}
}

void post_processor::eval_port(size_t port_num, size_t mode, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance->sim_ports.bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	std::ofstream ofs(std::format("port_{}_mode_{}.txt", port_num, mode));

	for (const auto& p : points)
	{
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance->sim_ports.entity_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance->nodes,
			e, { p.u, p.v }, sim_instance->port_dof_maps[port_num], sim_instance->port_eigen_vectors[port_num].col(mode));
		ofs << result_formatter::field_2d_at_point(p, elem_field);
	}
}

void post_processor::eval_full(size_t num_x, size_t num_y, size_t num_z)
{
	for (size_t p = 0; p < sim_instance->sim_ports.entity_ids.size(); p++)
	{
		eval_full(p, num_x, num_y, num_z);
	}
}

void post_processor::eval_full(size_t port_num, size_t num_x, size_t num_y, size_t num_z)
{
	auto points = generate_grid_points(sim_instance->bbox, num_x, num_y, num_z);

	std::ofstream ofs(std::format("full_port_{}_solution.txt", port_num));

	for (const auto& p : points)
	{
		auto e = mesher_interface::get_volume_element_by_coordinate(p);
		if (!e.has_value())
		{
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance->nodes,
			e.value(), p, sim_instance->full_dof_map, sim_instance->full_solutions[port_num]);

		ofs << result_formatter::field_3d_at_point(p, elem_field);
	}
	ofs.close();
}

void post_processor::eval_slice(slice_plane slice, size_t port_num, size_t num_u, size_t num_v, double w)
{
	rectangle plane; 
	if (slice == slice_plane::XY)
	{
		plane = rectangle(sim_instance->bbox.xmin, sim_instance->bbox.ymin,
			sim_instance->bbox.xmax, sim_instance->bbox.ymax);
	} 
	else if (slice == slice_plane::XZ)
	{
		plane = rectangle(sim_instance->bbox.xmin, sim_instance->bbox.zmin,
			sim_instance->bbox.xmax, sim_instance->bbox.zmax);
	} 
	else
	{
		plane = rectangle(sim_instance->bbox.ymin, sim_instance->bbox.zmin,
			sim_instance->bbox.ymax, sim_instance->bbox.zmax);
	}

	auto points = generate_grid_points(plane, num_u, num_v);
	std::ofstream ofs(std::format("slice_port_{}.txt", port_num));

	for (const auto& p : points)
	{

		point_3d p3d;
		if (slice == slice_plane::XY)
		{
			p3d = { p.u, p.v, w };
		}
		else if (slice == slice_plane::XZ)
		{
			p3d = { p.u, w, p.v };
		}
		else
		{
			p3d = { w, p.u, p.v };
		}

		auto e = mesher_interface::get_volume_element_by_coordinate(p3d);
		if (!e.has_value())
		{
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance->nodes,
			e.value(), p3d, sim_instance->full_dof_map, sim_instance->full_solutions[port_num]);

		if (slice == slice_plane::XY)
		{
			Eigen::Vector2cd surface_vec;
			surface_vec << elem_field(0), elem_field(1);
			ofs << result_formatter::field_2d_at_point(p, surface_vec);
		}
		else if (slice == slice_plane::XZ)
		{
			Eigen::Vector2cd surface_vec;
			surface_vec << elem_field(0), elem_field(2);
			ofs << result_formatter::field_2d_at_point(p, surface_vec);
		}
		else
		{
			Eigen::Vector2cd surface_vec;
			surface_vec << elem_field(1), elem_field(2);
			ofs << result_formatter::field_2d_at_point(p, surface_vec);
		}
	}
	ofs.close();
}