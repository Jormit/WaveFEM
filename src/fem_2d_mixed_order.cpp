#include "fem.h"
#include "quad.h"
#include "mesher_interface.h"
#include <iostream>

Eigen::Matrix<double, 8, 2> fem::_2d::mixed_order::basis(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda)
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

Eigen::Matrix<double, 8, 1> fem::_2d::mixed_order::basis_curl(const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda) {
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

std::pair<Eigen::Matrix<double, 8, 8>, Eigen::Matrix<double, 8, 8>>
fem::_2d::mixed_order::S_T(const Eigen::Matrix<double, 3, 2>& coords)
{
	Eigen::Matrix<double, 3, 3> simplex_coeff = fem::_2d::simplex_coefficients(coords);
	Eigen::Matrix<double, 3, 2> nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<double, 8, 8> S = Eigen::Matrix<double, 8, 8>::Zero();
	Eigen::Matrix<double, 8, 8> T = Eigen::Matrix<double, 8, 8>::Zero();
	for (int p = 0; p < 6; p++)
	{
		Eigen::Vector3d lambda;
		lambda << quad::surface::gauss_6_point[p][1], quad::surface::gauss_6_point[p][2], quad::surface::gauss_6_point[p][3];
		auto w = quad::surface::gauss_6_point[p][0];

		auto basis_curl = fem::_2d::mixed_order::basis_curl(lambda, nabla_lambda);
		auto basis = fem::_2d::mixed_order::basis(lambda, nabla_lambda);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				S(i, j) = S(i, j) + w * basis_curl(i) * basis_curl(j);
				T(i, j) = T(i, j) + w * basis.row(i).dot(basis.row(j));
			}
		}
	}

	auto area = fem::_2d::area(coords);
	return { S * area, T * area };
}

std::map<std::pair<size_t, size_t>, size_t> fem::_2d::mixed_order::dof_map(const std::vector<node>& nodes, const std::vector<tri>& elems)
{
	int i = 0;
	std::map<std::pair<size_t, size_t>, size_t> map;
	for (const auto& e : elems)
	{
		for (size_t edge = 0; edge < 3; edge++)
		{
			if (!map.contains({ e.edges[edge], 1 }))
			{
				auto edge_nodes = e.get_edge_nodes(edge);
				if (nodes[edge_nodes[0] - 1].type_2d != BOUNDARY_NODE || nodes[edge_nodes[1] - 1].type_2d != BOUNDARY_NODE)
				{
					map[{e.edges[edge], 1}] = i++;
					map[{e.edges[edge], 2}] = i++;
				}
			}
		}

		if (!map.contains({ e.face, 3 }))
		{
			map[{e.face, 3}] = i++;
			map[{e.face, 4}] = i++;
		}
	}
	return map;
}

std::pair<size_t, size_t> fem::_2d::mixed_order::global_dof_pair(const tri& elem, const size_t& dof_num)
{
	switch (dof_num) {
	case 0: return { elem.edges[0], 1 };
	case 1: return { elem.edges[1], 1 };
	case 2: return { elem.edges[2], 1 };
	case 3: return { elem.edges[0], 2 };
	case 4: return { elem.edges[1], 2 };
	case 5: return { elem.edges[2], 2 };
	case 6: return { elem.face    , 3 };
	case 7: return { elem.face    , 4 };
	}
}

std::pair<Eigen::SparseMatrix<double>, Eigen::SparseMatrix<double>>
fem::_2d::mixed_order::assemble_S_T(const std::vector<node>& nodes, const std::vector<tri>& elems,
	const std::map<std::pair<size_t, size_t>, size_t>& dof_map)
{
	Eigen::SparseMatrix<double> S_global(dof_map.size(), dof_map.size());
	Eigen::SparseMatrix<double> T_global(dof_map.size(), dof_map.size());

	for (const auto& e : elems)
	{
		Eigen::Matrix<double, 3, 2> coords;
		coords <<
			nodes[e.nodes[0] - 1].point_2d->u, nodes[e.nodes[0] - 1].point_2d->v,
			nodes[e.nodes[1] - 1].point_2d->u, nodes[e.nodes[1] - 1].point_2d->v,
			nodes[e.nodes[2] - 1].point_2d->u, nodes[e.nodes[2] - 1].point_2d->v;

		Eigen::Matrix<double, 8, 8> S_local, T_local;
		std::tie(S_local, T_local) = S_T(coords);

		for (int local_dof_i = 0; local_dof_i < 8; local_dof_i++)
		{
			auto global_dof_pair_i = global_dof_pair(e, local_dof_i);
			if (!dof_map.contains(global_dof_pair_i)) continue;
			auto global_dof_i = dof_map.at(global_dof_pair_i);
			for (int local_dof_j = 0; local_dof_j < 8; local_dof_j++)
			{
				auto global_dof_pair_j = global_dof_pair(e, local_dof_j);
				if (!dof_map.contains(global_dof_pair_j)) continue;
				auto global_dof_j = dof_map.at(global_dof_pair_j);

				S_global.coeffRef(global_dof_i, global_dof_j) = S_global.coeff(global_dof_i, global_dof_j) + S_local(local_dof_i, local_dof_j);
				T_global.coeffRef(global_dof_i, global_dof_j) = T_global.coeff(global_dof_i, global_dof_j) + T_local(local_dof_i, local_dof_j);;
			}
		}
	}

	S_global.makeCompressed();
	T_global.makeCompressed();

	return { S_global, T_global };
}

Eigen::Vector2d fem::_2d::mixed_order::eval_elem(const std::vector<node>& nodes, const tri& e, const point_2d& eval_point,
	const std::map<std::pair<size_t, size_t>, size_t>& dof_map, const Eigen::VectorXd& solution)
{
	Eigen::Matrix<double, 3, 2> coords;
	coords <<
		nodes[e.nodes[0] - 1].point_2d->u, nodes[e.nodes[0] - 1].point_2d->v,
		nodes[e.nodes[1] - 1].point_2d->u, nodes[e.nodes[1] - 1].point_2d->v,
		nodes[e.nodes[2] - 1].point_2d->u, nodes[e.nodes[2] - 1].point_2d->v;

	Eigen::Vector2d modified_eval_point;
	modified_eval_point << eval_point.u, eval_point.v;

	auto simplex_coeff = fem::_2d::simplex_coefficients(coords);
	auto nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);
	auto lambda = fem::_2d::lambda(modified_eval_point, simplex_coeff);

	return eval_elem(e, lambda, nabla_lambda, dof_map, solution);
}

Eigen::Vector2d fem::_2d::mixed_order::eval_elem(
	const tri& e, const Eigen::Vector3d& lambda, const Eigen::Matrix<double, 3, 2>& nabla_lambda,
	const std::map<std::pair<size_t, size_t>, size_t>& dof_map, const Eigen::VectorXd& solution)
{
	auto func = basis(lambda, nabla_lambda);

	Eigen::Vector2d value = Eigen::Vector2d::Zero();
	for (int i = 0; i < 8; i++)
	{
		auto dof_pair = global_dof_pair(e, i);
		if (dof_map.contains(dof_pair))
		{
			value += func.row(i) * solution(dof_map.at(dof_pair));
		}
	}
	return value;
}
