#include <iostream>
#include "mesher_interface.h"
#include "fem.h"
#include "setup.h"

int main()
{
	mesher_interface::initialize();

	int model_id = mesher_interface::import_model("../../../data/Horn.IGS");	
	auto port_points = setup::load_port_setup("../../../data/Horn.ports");
	
	auto bbox = mesher_interface::get_bounding_box();
	bbox.add_padding(20, 20, 20);
	int bbox_id = mesher_interface::add_box(bbox);

	mesher_interface::subtract(bbox_id, model_id);
	mesher_interface::mesh_model(100, 100);
	mesher_interface::view_model();
	
	auto node_map = mesher_interface::get_node_map();
	auto nodes = mesher_interface::get_all_nodes(node_map);
	auto volume_elements = mesher_interface::get_volume_elems(node_map);

	auto port_ids = mesher_interface::get_surface_ids_from_coms(port_points);
	auto port_surface_elements = mesher_interface::get_surface_elems_by_id(port_ids, node_map);

	for (auto n : port_surface_elements[0])
	{
		for (auto node : n.nodes)
		{
			nodes[node].tag = 2;
			std::cout << node << std::endl;
		}
	}

	for (auto elem : volume_elements)
	{
		if (nodes[elem.nodes[0]].tag == 2)
		{
			std::cout << "Yes" << std::endl;
		}
		if (nodes[elem.nodes[1]].tag == 2)
		{
			std::cout << "Yes" << std::endl;
		}
		if (nodes[elem.nodes[2]].tag == 2)
		{
			std::cout << "Yes" << std::endl;
		}
		if (nodes[elem.nodes[3]].tag == 2)
		{
			std::cout << "Yes" << std::endl;
		}
	}

	return 0;
}