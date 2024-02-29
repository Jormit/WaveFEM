#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <Eigen/Core>
#include <Eigen/dense>
#include <complex>

namespace helpers
{
	template <typename T>
	inline bool is_approx_equal(T x, T y)
	{
		const T epsilon = 1e-5;
		return std::abs(x - y) <= epsilon;
	}

	template <typename T>
	std::vector<T> flatten_vector(const std::vector<std::vector<T>>& v) {
		std::size_t total_size = 0;
		for (const auto& sub : v) total_size += sub.size();
		std::vector<T> result;
		result.reserve(total_size);
		for (const auto& sub : v) result.insert(result.end(), sub.begin(), sub.end());
		return result;
	}

	template <typename T, typename K>
	std::unordered_map<T, K> set_to_map(std::unordered_set<T> set, K initial)
	{
		std::unordered_map<T, K> new_map;
		for (auto elem : set)
		{
			new_map[elem] = initial;
		}
		return new_map;
	}

	template <typename Derived>
	std::complex<double> rowise_2d_dot_product(const Eigen::MatrixBase<Derived>& A, const Eigen::MatrixBase<Derived>& B)
	{
		std::complex<double> result = 0;
		for (size_t i = 0; i < A.rows(); i++)
		{
			result += A.row(i).dot(B.row(i));
		}
		return result;
	}

	template <typename Derived>
	std::vector<std::complex<double>> flatten_eigen_object_to_vector(const Eigen::MatrixBase<Derived>& a)
	{
		return std::vector<std::complex<double>>(a.derived().data(), a.derived().data() + a.rows() * a.cols());
	}
}