#include <iostream>
#include "mesher_interface.h"
#include "fem.h"
#include "../include/gmsh.h_cwrap"

int main()
{
	mesher_interface::initialize();
	int id1 = mesher_interface::import_model("../../../data/Original EH Model 1181-LI-T Solid Model.IGS");
	
	auto bbox = mesher_interface::get_bounding_box();
	bbox.add_padding(20, 20, 20);
	int id2 = mesher_interface::add_box(bbox);

	auto id3 = mesher_interface::subtract(id2, id1);
	mesher_interface::mesh_model(10, 10);
	mesher_interface::view_model();

	auto nodes = mesher_interface::get_nodes();
	auto tets = mesher_interface::get_elems();


	Eigen::Matrix<double, 3, 2> coords;
	coords << 0, 0,
		      1, 0,
		      0, 1;

	Eigen::Vector2d coord(0.25, 0.25);

	auto coeff = fem::_2d::simplex_coefficients(coords);
	auto lambda = fem::_2d::lambda(coord, coeff);
	auto nabla_lambda = fem::_2d::nabla_lambda(coeff);

	std::cout << coeff << std::endl;
	std::cout << nabla_lambda << std::endl;


	return 0;
}