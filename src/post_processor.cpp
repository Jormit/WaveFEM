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
		eval_port(p, num_x, num_y);
	}
}

void post_processor::eval_port(size_t port_num, size_t num_x, size_t num_y)
{
	auto bounds = sim_instance->sim_ports.bounds[port_num];
	bounds.add_padding(-1, -1);
	auto points = generate_grid_points(bounds, num_x, num_y);

	std::ofstream ofs(std::format("port_{}_solution.txt", port_num));

	for (const auto& p : points)
	{
		auto e = mesher_interface::get_surface_element_by_parametric_coordinate(p, sim_instance->sim_ports.entity_ids[port_num]);
		auto elem_field = fem::_2d::mixed_order::eval_elem(sim_instance->nodes,
			e, { p.u, p.v }, sim_instance->port_dof_maps[port_num], sim_instance->port_eigen_vectors[port_num].col(0));
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

void post_processor::eval_xslice(size_t port_num, size_t num_x, size_t num_y, double x)
{
	rectangle plane(sim_instance->bbox.ymin, sim_instance->bbox.zmin, 
		sim_instance->bbox.ymax, sim_instance->bbox.zmax);
	auto points = generate_grid_points(plane, num_x, num_y);

	std::ofstream ofs(std::format("slice_port_{}.txt", port_num));

	for (const auto& p : points)
	{
		point_3d p3d = { x, p.u, p.v };
		auto e = mesher_interface::get_volume_element_by_coordinate(p3d);
		if (!e.has_value())
		{
			continue;
		}

		auto elem_field = fem::_3d::mixed_order::eval_elem(sim_instance->nodes,
			e.value(), p3d, sim_instance->full_dof_map, sim_instance->full_solutions[port_num]);

		ofs << result_formatter::field_3d_at_point(p3d, elem_field);
	}

	ofs.close();
}