#include <vector>
#include "geometry.h"

// This class manages all the elements within a simulation context
class sim
{
public:
	sim(std::vector<node> nodes, std::vector<tet> volume_elems, std::vector<int> port_surface_ids, std::vector<std::vector<tri>> port_elems);

private:
	std::vector<node> nodes;
	std::vector<tet> volume_elems;
	std::vector<int> port_surface_ids;
	std::vector<std::vector<tri>> port_elems;
};