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

namespace pml
{
	pml_boundary create(double pml_box_padding);
	void label_elements(pml_boundary pml_volumes, std::vector<tet>& elems);
}

