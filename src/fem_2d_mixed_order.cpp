#include <iostream>

#include "fem.h"
#include "quad.h"
#include "mesher_interface.h"

Eigen::Matrix<double, 8, 2> fem::_2d::mixed_order::vector_basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	Eigen::Matrix<double, 8, 2> func;

	func.row(0) = lambda(0) * nabla_lambda.row(1) - lambda(1) * nabla_lambda.row(0);
	func.row(1) = lambda(0) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(0);
	func.row(2) = lambda(1) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(1);
	func.row(3) = lambda(0) * nabla_lambda.row(1) + lambda(1) * nabla_lambda.row(0);
	func.row(4) = lambda(0) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(0);
	func.row(5) = lambda(1) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(1);
	func.row(6) = (lambda(1) * lambda(2) * nabla_lambda.row(0)) + (lambda(0) * lambda(2) * nabla_lambda.row(1)) - 2 * (lambda(0) * lambda(1) * nabla_lambda.row(2));
	func.row(7) = (lambda(2) * lambda(0) * nabla_lambda.row(1)) + (lambda(1) * lambda(0) * nabla_lambda.row(2)) - 2 * (lambda(1) * lambda(2) * nabla_lambda.row(0));

	return func;
}

Eigen::Matrix<double, 8, 1> fem::_2d::mixed_order::vector_basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda) {
	Eigen::Matrix<double, 8, 1> func;
	Eigen::Matrix<double, 3, 3> nabla_matrix;
	nabla_matrix.col(0) = nabla_lambda.col(0);
	nabla_matrix.col(1) = nabla_lambda.col(1);
	nabla_matrix.col(2).setZero();

	func(0) = 2 * nabla_matrix.row(0).cross(nabla_matrix.row(1))(2);
	func(1) = 2 * nabla_matrix.row(0).cross(nabla_matrix.row(2))(2);
	func(2) = 2 * nabla_matrix.row(1).cross(nabla_matrix.row(2))(2);
	func(3) = 0;
	func(4) = 0;
	func(5) = 0;
	func(6) = -3 * lambda(1) * nabla_matrix.row(0).cross(nabla_matrix.row(2))(2) - 3 * lambda(0) * nabla_matrix.row(1).cross(nabla_matrix.row(2))(2);
	func(7) = -3 * lambda(2) * nabla_matrix.row(1).cross(nabla_matrix.row(0))(2) - 3 * lambda(1) * nabla_matrix.row(2).cross(nabla_matrix.row(0))(2);

	return func;
}

Eigen::Matrix<double, 6, 1> fem::_2d::mixed_order::scalar_basis(const Eigen::Vector3d& lambda)
{
	Eigen::Matrix<double, 6, 1> func;
	func(0) = (2 * lambda(0) - 1) * lambda(0);
	func(1) = (2 * lambda(1) - 1) * lambda(1);
	func(2) = (2 * lambda(2) - 1) * lambda(2);
	func(3) = 4 * lambda(0) * lambda(1);
	func(4) = 4 * lambda(0) * lambda(2);
	func(5) = 4 * lambda(1) * lambda(2);

	return func;
}

Eigen::Matrix<double, 6, 2> fem::_2d::mixed_order::scalar_basis_grad(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
{
	Eigen::Matrix<double, 6, 2> func;
	func.row(0) = nabla_lambda.row(0) * (4 * lambda(0) - 1);
	func.row(1) = nabla_lambda.row(1) * (4 * lambda(1) - 1);
	func.row(2) = nabla_lambda.row(2) * (4 * lambda(2) - 1);
	func.row(3) = 4 * (nabla_lambda.row(0) * lambda(1) + nabla_lambda.row(1) * lambda(0));
	func.row(4) = 4 * (nabla_lambda.row(0) * lambda(2) + nabla_lambda.row(2) * lambda(0));
	func.row(5) = 4 * (nabla_lambda.row(1) * lambda(2) + nabla_lambda.row(2) * lambda(1));

	return func;
}

std::pair<Eigen::Matrix<double, 14, 14>, Eigen::Matrix<double, 14, 14>>
fem::_2d::mixed_order::A_B(const Eigen::Matrix<double, 3, 2>& coords, material mat, double k0)
{
	Eigen::Matrix<double, 3, 3> simplex_coeff = fem::_2d::simplex_coefficients(coords);
	Eigen::Matrix<double, 3, 2> nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<double, 14, 14> A = Eigen::Matrix<double, 14, 14>::Zero();
	Eigen::Matrix<double, 14, 14> B = Eigen::Matrix<double, 14, 14>::Zero();

	auto ep = mat.permittivity(0, 0).real();
	auto mu = mat.permeability(0, 0).real();

	for (size_t p = 0; p < 6; p++)
	{
		Eigen::Vector3d lambda;
		lambda << 
			quad::surface::gauss_6_point[p][1],
			quad::surface::gauss_6_point[p][2],
			quad::surface::gauss_6_point[p][3];
		auto w = quad::surface::gauss_6_point[p][0];

		auto vector_basis = fem::_2d::mixed_order::vector_basis(lambda, nabla_lambda);
		auto vector_basis_curl = fem::_2d::mixed_order::vector_basis_curl(lambda, nabla_lambda);

		auto scalar_basis = fem::_2d::mixed_order::scalar_basis(lambda);
		auto scalar_basis_grad = fem::_2d::mixed_order::scalar_basis_grad(lambda, nabla_lambda);


		for (size_t i = 0; i < 8; i++)
		{
			for (size_t j = 0; j < 8; j++)
			{
				A(i, j) += 
					w * (vector_basis_curl(i) * vector_basis_curl(j) / mu 
					- k0 * k0 * ep * vector_basis.row(i).dot(vector_basis.row(j)));
				B(i, j) += w * vector_basis.row(i).dot(vector_basis.row(j)) / mu;
			}
			for (size_t j = 8; j < 14; j++)
			{
				B(i, j) += w * vector_basis.row(i).dot(scalar_basis_grad.row(j - 8)) / mu;
			}
		}

		for (size_t i = 8; i < 14; i++)
		{
			for (size_t j = 0; j < 8; j++)
			{
				B(i, j) += w * scalar_basis_grad.row(i - 8).dot(vector_basis.row(j)) / mu;
			}
			for (size_t j = 8; j < 14; j++)
			{
				B(i, j) += 
					w * (scalar_basis_grad.row(i - 8).dot(scalar_basis_grad.row(j - 8)) / mu 
					- k0 * k0 * ep * scalar_basis.row(i - 8).dot(scalar_basis.row(j - 8)));
			}
		}
	}

	auto area = fem::_2d::area(coords);
	return { A * area, B * area };
}

fem::dof_map fem::_2d::mixed_order::generate_dof_map(const std::vector<geo::node>& nodes,
	const std::vector<geo::tri>& elems, std::unordered_map<size_t, int> boundary_edge_map)
{
	int i = 0;
	fem::dof_map map;
	for (const auto& e : elems)
	{
		for (size_t edge = 0; edge < 3; edge++)
		{
			auto global_edge = e.edges[edge];
			if (!map.contains({ global_edge, fem::dof_type::EDGE_1 }))
			{
				if (boundary_edge_map[global_edge] != geo::PORT_OUTER_BOUNDARY)
				{
					map[{global_edge, fem::dof_type::EDGE_1}] = i++;
					map[{global_edge, fem::dof_type::EDGE_2}] = i++;

					map[{global_edge, fem::dof_type::NODE_2}] = i++;
				}
			}
		}

		if (!map.contains({ e.face, fem::dof_type::FACE_1 }))
		{
			map[{e.face, fem::dof_type::FACE_1}] = i++;
			map[{e.face, fem::dof_type::FACE_2}] = i++;
		}

		for (size_t node = 0; node < 3; node++)
		{
			auto global_node = e.nodes[node];
			if (!map.contains({ global_node, fem::dof_type::NODE_1 }))
			{
				if (!nodes[global_node - 1].boundary_2d)
				{
					map[{global_node, fem::dof_type::NODE_1}] = i++;
				}
			}
		}
	}
	return map;
}

fem::dof_pair fem::_2d::mixed_order::global_dof_pair(const geo::tri& elem, const size_t& dof_num)
{
	switch (dof_num) {
	case 0: return  { elem.edges[0], fem::dof_type::EDGE_1 };
	case 1: return  { elem.edges[1], fem::dof_type::EDGE_1 };
	case 2: return  { elem.edges[2], fem::dof_type::EDGE_1 };
	case 3: return  { elem.edges[0], fem::dof_type::EDGE_2 };
	case 4: return  { elem.edges[1], fem::dof_type::EDGE_2 };
	case 5: return  { elem.edges[2], fem::dof_type::EDGE_2 };
	case 6: return  { elem.face    , fem::dof_type::FACE_1 };
	case 7: return  { elem.face    , fem::dof_type::FACE_2 };
	case 8: return  { elem.nodes[0], fem::dof_type::NODE_1 };
	case 9: return  { elem.nodes[1], fem::dof_type::NODE_1 };
	case 10: return { elem.nodes[2], fem::dof_type::NODE_1 };
	case 11: return { elem.edges[0], fem::dof_type::NODE_2 };
	case 12: return { elem.edges[1], fem::dof_type::NODE_2 };
	case 13: return { elem.edges[2], fem::dof_type::NODE_2 };
	}
	exit(1);
}

std::pair<Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>>
fem::_2d::mixed_order::assemble_A_B(const std::vector<geo::node>& nodes, const std::vector<geo::tri>& elems,
	std::vector<material> materials, const fem::dof_map& dof_map, double k0)
{
	Eigen::SparseMatrix<double> A_global(dof_map.size(), dof_map.size());
	Eigen::SparseMatrix<double> B_global(dof_map.size(), dof_map.size());

	for (const auto& e : elems)
	{
		Eigen::Matrix<double, 3, 2> coords = e.coordinate_matrix(nodes);
		
		auto mat = materials[e.material_id];
		if (!mat.isotropic)
		{
			std::cerr << "Anisotropic material on ports is not supported!" << std::endl;
			exit(1);
		}

		Eigen::Matrix<double, 14, 14> A_local, B_local;
		std::tie(A_local, B_local) = A_B(coords, mat, k0);

		for (size_t local_dof_i = 0; local_dof_i < 14; local_dof_i++)
		{
			auto global_dof_pair_i = global_dof_pair(e, local_dof_i);
			if (!dof_map.contains(global_dof_pair_i)) continue;
			auto global_dof_i = dof_map.at(global_dof_pair_i);
			for (size_t local_dof_j = 0; local_dof_j < 14; local_dof_j++)
			{
				auto global_dof_pair_j = global_dof_pair(e, local_dof_j);
				if (!dof_map.contains(global_dof_pair_j)) continue;
				auto global_dof_j = dof_map.at(global_dof_pair_j);

				A_global.coeffRef(global_dof_i, global_dof_j) += A_local(local_dof_i, local_dof_j);
				B_global.coeffRef(global_dof_i, global_dof_j) += B_local(local_dof_i, local_dof_j);
			}
		}
	}

	A_global.makeCompressed();
	B_global.makeCompressed();

	return { A_global, B_global };
}

Eigen::Vector2cd fem::_2d::mixed_order::eval_elem(const std::vector<geo::node>& nodes, const geo::tri& e,
	const geo::point_2d& eval_point, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	Eigen::Matrix<double, 3, 2> coords = e.coordinate_matrix(nodes);
	Eigen::Vector2d modified_eval_point = eval_point.to_Eigen();

	auto simplex_coeff = fem::_2d::simplex_coefficients(coords);
	auto nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);
	auto lambda = fem::_2d::lambda(modified_eval_point, simplex_coeff);

	return eval_elem(e, lambda, nabla_lambda, dof_map, solution);
}

Eigen::Vector2cd fem::_2d::mixed_order::eval_elem(const geo::tri& e, const Eigen::Vector3d& lambda,
	const Eigen::Matrix<double, 3, 2>& nabla_lambda, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	auto func = vector_basis(lambda, nabla_lambda);

	Eigen::Vector2cd value = Eigen::Vector2d::Zero();
	for (size_t i = 0; i < 8; i++)
	{
		auto dof_pair = global_dof_pair(e, i);
		if (dof_map.contains(dof_pair))
		{
			value += func.row(i) * solution(dof_map.at(dof_pair));
		}
	}
	return value;
}
