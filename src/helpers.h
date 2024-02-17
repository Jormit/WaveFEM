#pragma once
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>

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
	
}