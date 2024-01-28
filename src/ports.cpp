#include "ports.h"
#include "mesher_interface.h"
#include <fstream>
#include <sstream>

ports::ports(std::vector<point_3d> points)
{
	port_points = points;
	entity_ids = mesher_interface::get_surface_from_com(port_points);
	elements = mesher_interface::get_surface_elems(entity_ids);
	bounds = mesher_interface::get_surface_bounds(entity_ids);
}

void ports::update_node_tags(std::vector<node>& nodes)
{
	for (int p = 0; p < entity_ids.size(); p++)
	{
		for (const auto& e : elements[p])
		{
			for (auto n : e.nodes)
			{
				nodes[n - 1].type_3d = p + 1;			
			}
		}
	}
}
