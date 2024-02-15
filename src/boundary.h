#include <array>
#include <utility>

#include "geometry.h"
#include "mesher_interface.h"

struct pml_boundary
{
	std::array<int, 2> x_ids;
	std::array<int, 2> y_ids;
	std::array<int, 2> z_ids;

	std::array<int, 4> xy_ids;
	std::array<int, 4> xz_ids;
	std::array<int, 4> yz_ids;

	std::array<int, 8> xyz_ids;
};

namespace boundary
{
	pml_boundary create_pml(box pml_bounds, box free_space_bounds);
	pml_boundary setup_pml_boundary(point_3d pml_box_padding);
	std::vector<tet> get_pml_elements(pml_boundary pml_volumes);
}

