//@	{"dependencies_extra":[{"ref":"./search.o", "rel":"implementation"}]}

#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include "./rectangle.hpp"

#include <cmath>
#include <vector>
#include <utility>

namespace cheapest_route
{
	enum class node_type : int{source, target};

	template<class T>
	using from = vec<T, 2, node_type::source>;

	template<class T>
	using to = vec<T, 2, node_type::target>;

	enum class quantity_type:int{vector, point};

	template<class T>
	constexpr auto operator-(to<T> a, from<T> b)
	{ return vec<double, 2, quantity_type::vector>{a} - vec<double, 2, quantity_type::vector>{b}; }

	struct flat_euclidian_norm
	{
		constexpr auto operator()(vec<double, 2, quantity_type::vector> dx,
			vec<double, 2, quantity_type::point>) const
		{ return std::sqrt(length_squared(dx)); }
	};

	struct homogeous_cost
	{
		static constexpr auto cost = 1.0;

		constexpr auto operator()(vec<double, 2, quantity_type::point>) const
		{ return cost; }
	};

	struct visited_node
	{
		vec<double, 2, quantity_type::point> loc;
		double total_cost;
	};

	using path = std::vector<visited_node>;

	using cost_function_ptr = double (*)(void const* callback_data,
		vec<double, 2, quantity_type::vector>,
		vec<double, 2, quantity_type::point>);

	path search_impl(from<int64_t> source,
		to<int64_t> target,
		dimensions_2d<int64_t, boundary_type::inclusive, boundary_type::exclusive, boundary_type::inclusive, boundary_type::exclusive> const& domain,
		void const* callback_data,
		cost_function_ptr cost_function);

	template<class CostFunction = homogeous_cost, class Metric = flat_euclidian_norm>
	auto search(from<int64_t> source,
		to<int64_t> target,
		dimensions_2d<int64_t, boundary_type::inclusive, boundary_type::exclusive, boundary_type::inclusive, boundary_type::exclusive>  const& domain,
		CostFunction&& f = homogeous_cost{},
		Metric&& ds = flat_euclidian_norm{})
	{
		std::pair functions{std::forward<CostFunction>(f), std::forward<Metric>(ds)};
		return search_impl(source, target, domain, &functions, [](void const* func_pair,
			vec<double, 2, quantity_type::vector> dx,
			vec<double, 2, quantity_type::point> x){
			auto const& data = *static_cast<decltype(functions) const*>(func_pair);
			return data.first(x)*data.second(dx, x);
		});
	}
}

#endif