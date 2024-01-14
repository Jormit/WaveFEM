#include "sim.h"

sim::sim(std::vector<node> nodes, std::vector<tet> volume_elems, std::vector<int> port_surface_ids, std::vector<std::vector<tri>> port_elems) :
	nodes(nodes), volume_elems(volume_elems), port_surface_ids(port_surface_ids), port_elems(port_elems)
{}