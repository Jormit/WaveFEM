#pragma once
#include <vector>
#include <list>

namespace helpers
{
	inline bool isEqual(double x, double y)
	{
		const double epsilon = 1e-5;
		return std::abs(x - y) <= epsilon;
	}

	// https://stackoverflow.com/questions/17294629/merging-flattening-sub-vectors-into-a-single-vector-c-converting-2d-to-1d
	template <typename T>
	std::vector<T> flatten_vector(const std::vector<std::vector<T>>& v) {
		std::size_t total_size = 0;
		for (const auto& sub : v) total_size += sub.size();
		std::vector<T> result;
		result.reserve(total_size);
		for (const auto& sub : v) result.insert(result.end(), sub.begin(), sub.end());
		return result;
	}

	template <typename T>
	bool check_for_common_elements(T list1, T list2) {
		using B = typename T::value_type;
		for (B x : list1) for (B y : list2) if (x == y) return true;
		return false;
	}

	template <typename T>
	bool check_for_common_elements(T list1, T list2, T list3) {
		using B = typename T::value_type;
		for (B x : list1) for (B y : list2) for (B z : list3) if (x == y && x == z && z == y) return true;
		return false;
	}
}