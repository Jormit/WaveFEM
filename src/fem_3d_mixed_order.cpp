#include <iostream>
#include <list>

#include "fem.h"
#include "quad.h"
#include "helpers.h"

Eigen::Matrix<double, 20, 3> fem::_3d::mixed_order::basis(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda)
{
	Eigen::Matrix<double, 20, 3> func;

	// CT/LN Edge Functions 
	func.row(0) = lambda(0) * nabla_lambda.row(1) - lambda(1) * nabla_lambda.row(0);
	func.row(1) = lambda(0) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(0);
	func.row(2) = lambda(0) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(0);
	func.row(3) = lambda(1) * nabla_lambda.row(2) - lambda(2) * nabla_lambda.row(1);
	func.row(4) = lambda(1) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(1);
	func.row(5) = lambda(2) * nabla_lambda.row(3) - lambda(3) * nabla_lambda.row(2);

	// LT/LN Edge Functions
	func.row(6) = lambda(0) * nabla_lambda.row(1) + lambda(1) * nabla_lambda.row(0);
	func.row(7) = lambda(0) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(0);
	func.row(8) = lambda(0) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(0);
	func.row(9) = lambda(1) * nabla_lambda.row(2) + lambda(2) * nabla_lambda.row(1);
	func.row(10) = lambda(1) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(1);
	func.row(11) = lambda(2) * nabla_lambda.row(3) + lambda(3) * nabla_lambda.row(2);

	// LT/QN Face Functions
	func.row(12) = lambda(1) * lambda(2) * nabla_lambda.row(0) + lambda(0) * lambda(2) * nabla_lambda.row(1) - 2 * lambda(0) * lambda(1) * nabla_lambda.row(2);
	func.row(13) = lambda(1) * lambda(3) * nabla_lambda.row(0) + lambda(0) * lambda(3) * nabla_lambda.row(1) - 2 * lambda(0) * lambda(1) * nabla_lambda.row(3);
	func.row(14) = lambda(2) * lambda(3) * nabla_lambda.row(0) + lambda(0) * lambda(3) * nabla_lambda.row(2) - 2 * lambda(0) * lambda(2) * nabla_lambda.row(3);
	func.row(15) = lambda(2) * lambda(3) * nabla_lambda.row(1) + lambda(1) * lambda(3) * nabla_lambda.row(2) - 2 * lambda(1) * lambda(2) * nabla_lambda.row(3);

	func.row(16) = lambda(2) * lambda(0) * nabla_lambda.row(1) + lambda(1) * lambda(0) * nabla_lambda.row(2) - 2 * lambda(1) * lambda(2) * nabla_lambda.row(0);
	func.row(17) = lambda(3) * lambda(0) * nabla_lambda.row(1) + lambda(1) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(1) * lambda(3) * nabla_lambda.row(0);
	func.row(18) = lambda(3) * lambda(0) * nabla_lambda.row(2) + lambda(2) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(2) * lambda(3) * nabla_lambda.row(0);
	func.row(19) = lambda(3) * lambda(1) * nabla_lambda.row(2) + lambda(2) * lambda(0) * nabla_lambda.row(3) - 2 * lambda(2) * lambda(3) * nabla_lambda.row(1);

	return func;
}

Eigen::Matrix<double, 20, 3> fem::_3d::mixed_order::basis_curl(const Eigen::Vector4d& lambda, const Eigen::Matrix<double, 4, 3>& nabla_lambda)
{
	Eigen::Matrix<double, 20, 3> func;

	// CT/LN Edge Functions 
	func.row(0) = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(1));
	func.row(1) = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(2));
	func.row(2) = 2 * nabla_lambda.row(0).cross(nabla_lambda.row(3));
	func.row(3) = 2 * nabla_lambda.row(1).cross(nabla_lambda.row(2));
	func.row(4) = 2 * nabla_lambda.row(1).cross(nabla_lambda.row(3));
	func.row(5) = 2 * nabla_lambda.row(2).cross(nabla_lambda.row(3));

	// LT/LN Edge Functions
	func.row(6) = Eigen::Vector3d::Zero();
	func.row(7) = Eigen::Vector3d::Zero();
	func.row(8) = Eigen::Vector3d::Zero();
	func.row(9) = Eigen::Vector3d::Zero();
	func.row(10) = Eigen::Vector3d::Zero();
	func.row(11) = Eigen::Vector3d::Zero();

	// LT/QN Face Functions
	func.row(12) = -3 * lambda(1) * nabla_lambda.row(0).cross(nabla_lambda.row(2)) - 3 * lambda(0) * nabla_lambda.row(1).cross(nabla_lambda.row(2));
	func.row(13) = -3 * lambda(1) * nabla_lambda.row(0).cross(nabla_lambda.row(3)) - 3 * lambda(0) * nabla_lambda.row(1).cross(nabla_lambda.row(3));
	func.row(14) = -3 * lambda(2) * nabla_lambda.row(0).cross(nabla_lambda.row(3)) - 3 * lambda(0) * nabla_lambda.row(2).cross(nabla_lambda.row(3));
	func.row(15) = -3 * lambda(2) * nabla_lambda.row(1).cross(nabla_lambda.row(3)) - 3 * lambda(1) * nabla_lambda.row(2).cross(nabla_lambda.row(3));

	func.row(16) = -3 * lambda(2) * nabla_lambda.row(1).cross(nabla_lambda.row(0)) - 3 * lambda(1) * nabla_lambda.row(2).cross(nabla_lambda.row(0));
	func.row(17) = -3 * lambda(3) * nabla_lambda.row(1).cross(nabla_lambda.row(0)) - 3 * lambda(1) * nabla_lambda.row(3).cross(nabla_lambda.row(0));
	func.row(18) = -3 * lambda(3) * nabla_lambda.row(2).cross(nabla_lambda.row(0)) - 3 * lambda(2) * nabla_lambda.row(3).cross(nabla_lambda.row(0));
	func.row(19) = -3 * lambda(3) * nabla_lambda.row(2).cross(nabla_lambda.row(1)) - 3 * lambda(2) * nabla_lambda.row(3).cross(nabla_lambda.row(1));

	return func;
}

std::pair<Eigen::Matrix<std::complex<double>, 20, 20>, Eigen::Matrix<std::complex<double>, 20, 20>>
fem::_3d::mixed_order::S_T(const Eigen::Matrix<double, 4, 3>& coords, material mat)
{
	Eigen::Matrix<double, 4, 4> simplex_coeff = fem::_3d::simplex_coefficients(coords);
	Eigen::Matrix<double, 4, 3> nabla_lambda = fem::_3d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<std::complex<double>, 20, 20> S = Eigen::Matrix<std::complex<double>, 20, 20>::Zero();
	Eigen::Matrix<std::complex<double>, 20, 20> T = Eigen::Matrix<std::complex<double>, 20, 20>::Zero();

	for (size_t p = 0; p < 11; p++)
	{
		Eigen::Vector4d lambda;
		lambda << 
			quad::volume::gauss_11_point[p][1],
			quad::volume::gauss_11_point[p][2],
			quad::volume::gauss_11_point[p][3],
			quad::volume::gauss_11_point[p][4];
		auto w = quad::volume::gauss_11_point[p][0];

		auto curl_funcs = basis_curl(lambda, nabla_lambda);
		auto basis_funcs = basis(lambda, nabla_lambda);

		auto mu_inv = mat.permeability.inverse();
		auto ep = mat.permittivity;

		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				S(i, j) += w * (curl_funcs.row(i).transpose()).dot(mu_inv * curl_funcs.row(j).transpose());
				T(i, j) += w * (basis_funcs.row(i).transpose()).dot(ep * basis_funcs.row(j).transpose());
			}
		}
	}

	auto volume = fem::_3d::volume(coords);
	return { volume * S, volume * T };
}

Eigen::Matrix<double, 8, 8>	fem::_3d::mixed_order::B(const Eigen::Matrix<double, 3, 2>& coords)
{
	Eigen::Matrix<double, 3, 3> simplex_coeff = fem::_2d::simplex_coefficients(coords);
	Eigen::Matrix<double, 3, 2> nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<double, 8, 8> B = Eigen::Matrix<double, 8, 8>::Zero();

	for (size_t p = 0; p < 6; p++)
	{
		Eigen::Vector3d lambda;
		lambda << 
			quad::surface::gauss_6_point[p][1],
			quad::surface::gauss_6_point[p][2],
			quad::surface::gauss_6_point[p][3];
		auto w = quad::surface::gauss_6_point[p][0];

		auto basis_funcs = fem::_2d::mixed_order::vector_basis(lambda, nabla_lambda);

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				B(i, j) += w * basis_funcs.row(i).dot(basis_funcs.row(j));
			}
		}
	}

	auto area = fem::_2d::area(coords);
	return B * area;
}

Eigen::Matrix<std::complex<double>, 8, 1> fem::_3d::mixed_order::b(
	const geo::tri& e, const Eigen::Matrix<double, 3, 2>& coords,
	const dof_map& dof_map, const Eigen::VectorXd& solution)
{
	Eigen::Matrix<double, 3, 3> simplex_coeff = fem::_2d::simplex_coefficients(coords);
	Eigen::Matrix<double, 3, 2> nabla_lambda = fem::_2d::nabla_lambda(simplex_coeff);

	Eigen::Matrix<std::complex<double>, 8, 1> b = Eigen::Matrix<double, 8, 1>::Zero();

	for (size_t p = 0; p < 6; p++)
	{
		Eigen::Vector3d lambda;
		lambda << quad::surface::gauss_6_point[p][1], quad::surface::gauss_6_point[p][2], quad::surface::gauss_6_point[p][3];
		auto w = quad::surface::gauss_6_point[p][0];

		auto E_inc = fem::_2d::mixed_order::eval_elem(e, lambda, nabla_lambda, dof_map, solution);
		auto basis = fem::_2d::mixed_order::vector_basis(lambda, nabla_lambda);

		for (int i = 0; i < 8; i++)
		{
			b(i) += w * basis.row(i).dot(E_inc);
		}
	}

	auto area = fem::_2d::area(coords);
	return b * area;
}

fem::dof_map fem::_3d::mixed_order::generate_dof_map(const std::vector<geo::tet>& elems,
	std::unordered_map<size_t, int> boundary_edge_map, std::unordered_map<size_t, int> boundary_face_map)
{
	int i = 0;
	fem::dof_map map;
	for (const auto& e : elems)
	{
		for (size_t edge = 0; edge < 6; edge++)
		{
			auto global_edge = e.edges[edge];
			if (!map.contains({ global_edge, fem::dof_type::EDGE_1 }))
			{
				if (!boundary_edge_map.contains(global_edge) || boundary_edge_map[global_edge] > geo::BOUNDARY)
				{
					map[{e.edges[edge], fem::dof_type::EDGE_1}] = i++;
					map[{e.edges[edge], fem::dof_type::EDGE_2}] = i++;
				}
			}
		}

		for (size_t face = 0; face < 4; face++)
		{
			auto global_face = e.faces[face];
			if (!map.contains({ global_face, fem::dof_type::FACE_1 }))
			{
				if (!boundary_face_map.contains(global_face) || boundary_face_map[global_face] > geo::BOUNDARY)
				{
					map[{e.faces[face], fem::dof_type::FACE_1}] = i++;
					map[{e.faces[face], fem::dof_type::FACE_2}] = i++;
				}
			}
		}
	}
	return map;
}

fem::dof_pair fem::_3d::mixed_order::global_dof_pair(const geo::tet& elem, const size_t& dof_num)
{
	switch (dof_num) {
	case 0:  return { elem.edges[0], fem::dof_type::EDGE_1 };
	case 1:  return { elem.edges[1], fem::dof_type::EDGE_1 };
	case 2:  return { elem.edges[2], fem::dof_type::EDGE_1 };
	case 3:  return { elem.edges[3], fem::dof_type::EDGE_1 };
	case 4:  return { elem.edges[4], fem::dof_type::EDGE_1 };
	case 5:  return { elem.edges[5], fem::dof_type::EDGE_1 };
	case 6:  return { elem.edges[0], fem::dof_type::EDGE_2 };
	case 7:  return { elem.edges[1], fem::dof_type::EDGE_2 };
	case 8:  return { elem.edges[2], fem::dof_type::EDGE_2 };
	case 9:  return { elem.edges[3], fem::dof_type::EDGE_2 };
	case 10: return { elem.edges[4], fem::dof_type::EDGE_2 };
	case 11: return { elem.edges[5], fem::dof_type::EDGE_2 };
	case 12: return { elem.faces[0], fem::dof_type::FACE_1 };
	case 13: return { elem.faces[1], fem::dof_type::FACE_1 };
	case 14: return { elem.faces[2], fem::dof_type::FACE_1 };
	case 15: return { elem.faces[3], fem::dof_type::FACE_1 };
	case 16: return { elem.faces[0], fem::dof_type::FACE_2 };
	case 17: return { elem.faces[1], fem::dof_type::FACE_2 };
	case 18: return { elem.faces[2], fem::dof_type::FACE_2 };
	case 19: return { elem.faces[3], fem::dof_type::FACE_2 };
	}
	exit(1);
}

Eigen::SparseMatrix<std::complex<double>> fem::_3d::mixed_order::assemble_A(const std::vector<geo::node>& nodes,
	const std::vector<geo::tet>& elems, std::vector<material> materials, const std::vector<std::vector<geo::tri>>& boundary_elems,
	const dof_map& dof_map, std::complex<double> k0, std::vector<std::complex<double>> boundary_k)
{
	Eigen::SparseMatrix<std::complex<double>> A(dof_map.size(), dof_map.size());
	A.reserve(Eigen::VectorXi::Constant(dof_map.size(), 100));

	for (const auto& e : elems)
	{
		Eigen::Matrix<double, 4, 3> coords = e.coordinate_matrix(nodes);
		Eigen::Matrix<std::complex<double>, 20, 20> S_local, T_local;
		std::tie(S_local, T_local) = S_T(coords, materials[e.material_id]);

		for (size_t local_dof_i = 0; local_dof_i < 20; local_dof_i++)
		{
			auto global_dof_pair_i = global_dof_pair(e, local_dof_i);
			if (!dof_map.contains(global_dof_pair_i)) continue;
			auto global_dof_i = dof_map.at(global_dof_pair_i);
			for (size_t local_dof_j = 0; local_dof_j < 20; local_dof_j++)
			{
				auto global_dof_pair_j = global_dof_pair(e, local_dof_j);
				if (!dof_map.contains(global_dof_pair_j)) continue;
				auto global_dof_j = dof_map.at(global_dof_pair_j);
				A.coeffRef(global_dof_i, global_dof_j) +=
					S_local(local_dof_i, local_dof_j) - k0 * k0 * T_local(local_dof_i, local_dof_j);
			}
		}
	}

	for (size_t i = 0; i < boundary_elems.size(); i++)
	{
		for (const auto& e : boundary_elems[i])
		{
			Eigen::Matrix<double, 3, 2> coords = e.coordinate_matrix(nodes);
			auto B_local = B(coords);

			for (int local_dof_i = 0; local_dof_i < 8; local_dof_i++)
			{
				auto global_dof_pair_i = fem::_2d::mixed_order::global_dof_pair(e, local_dof_i);
				if (!dof_map.contains(global_dof_pair_i)) continue;
				auto global_dof_i = dof_map.at(global_dof_pair_i);
				for (size_t local_dof_j = 0; local_dof_j < 8; local_dof_j++)
				{
					auto global_dof_pair_j = fem::_2d::mixed_order::global_dof_pair(e, local_dof_j);
					if (!dof_map.contains(global_dof_pair_j)) continue;
					auto global_dof_j = dof_map.at(global_dof_pair_j);

					A.coeffRef(global_dof_i, global_dof_j) += boundary_k[i] * std::complex<double>{0, 1} * B_local(local_dof_i, local_dof_j);
				}
			}
		}
	}
	

	A.makeCompressed();

	return A;
}

Eigen::VectorXcd  fem::_3d::mixed_order::assemble_b(const std::vector<geo::node>& nodes, const std::vector<geo::tri>& surface_elems,
	const dof_map& solution_dof_map, const dof_map& excitation_dof_map, const Eigen::VectorXd& excitation, std::complex<double> ki)
{
	Eigen::VectorXcd b(solution_dof_map.size());
	b.setZero();

	for (const auto& e : surface_elems)
	{
		Eigen::Matrix<double, 3, 2> coords = e.coordinate_matrix(nodes);
		auto b_local = fem::_3d::mixed_order::b(e, coords, excitation_dof_map, excitation);

		for (size_t local_dof_i = 0; local_dof_i < 8; local_dof_i++)
		{
			auto global_dof_pair_i = fem::_2d::mixed_order::global_dof_pair(e, local_dof_i);
			if (!solution_dof_map.contains(global_dof_pair_i)) continue;
			auto global_dof_i = solution_dof_map.at(global_dof_pair_i);

			b(global_dof_i) += b_local(local_dof_i);
		}
	}
	return -2.0 * std::complex<double>({ 0,1 }) * ki * b;
}

Eigen::Vector3cd fem::_3d::mixed_order::eval_elem(const std::vector<geo::node>& nodes, const geo::tet& e,
	const geo::point_3d& eval_point, const dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	Eigen::Matrix<double, 4, 3> coords = e.coordinate_matrix(nodes);
	Eigen::Vector3d modified_eval_point = eval_point.to_Eigen();

	auto simplex_coeff = fem::_3d::simplex_coefficients(coords);
	auto nabla_lambda = fem::_3d::nabla_lambda(simplex_coeff);
	auto lambda = fem::_3d::lambda(modified_eval_point, simplex_coeff);

	return eval_elem(e, lambda, nabla_lambda, dof_map, solution);
}

Eigen::Vector3cd fem::_3d::mixed_order::eval_elem(const geo::tet& e, const Eigen::Vector4d& lambda,
	const Eigen::Matrix<double, 4, 3>& nabla_lambda, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	auto func = basis(lambda, nabla_lambda);

	Eigen::Vector3cd value = Eigen::Vector3cd::Zero();
	for (size_t i = 0; i < 20; i++)
	{
		auto dof_pair = global_dof_pair(e, i);
		if (dof_map.contains(dof_pair))
		{
			value += func.row(i) * solution(dof_map.at(dof_pair));
		}
	}
	return value;
}

Eigen::Vector3cd fem::_3d::mixed_order::eval_elem_curl(const std::vector<geo::node>& nodes, const geo::tet& e,
	const geo::point_3d& eval_point, const dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	Eigen::Matrix<double, 4, 3> coords = e.coordinate_matrix(nodes);
	Eigen::Vector3d modified_eval_point = eval_point.to_Eigen();

	auto simplex_coeff = fem::_3d::simplex_coefficients(coords);
	auto nabla_lambda = fem::_3d::nabla_lambda(simplex_coeff);
	auto lambda = fem::_3d::lambda(modified_eval_point, simplex_coeff);

	return eval_elem_curl(e, lambda, nabla_lambda, dof_map, solution);
}

Eigen::Vector3cd fem::_3d::mixed_order::eval_elem_curl(const geo::tet& e, const Eigen::Vector4d& lambda,
	const Eigen::Matrix<double, 4, 3>& nabla_lambda, const fem::dof_map& dof_map, const Eigen::VectorXcd& solution)
{
	auto func = basis_curl(lambda, nabla_lambda);

	Eigen::Vector3cd value = Eigen::Vector3cd::Zero();
	for (size_t i = 0; i < 20; i++)
	{
		auto dof_pair = global_dof_pair(e, i);
		if (dof_map.contains(dof_pair))
		{
			value += func.row(i) * solution(dof_map.at(dof_pair));
		}
	}

	return value;
}