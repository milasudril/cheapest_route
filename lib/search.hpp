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

	template<class T>
	constexpr auto operator-(to<T> a, from<T> b)
	{ return vec<double, 2, quantity_type::vector>{a} - vec<double, 2, quantity_type::vector>{b}; }

	template<class T>
	constexpr auto midpoint(to<T> a, from<T> b)
	{ return vec<double, 2, quantity_type::point>{0.5*(vec<double, 2>{a} + vec<double, 2>{b})}; }

	struct flat_euclidian_norm
	{
		constexpr auto operator()(from<double> x0, to<double> x1) const
		{ return std::sqrt(length_squared(x1 - x0)); }
	};

	struct visited_node
	{
		vec<double, 2, quantity_type::point> loc;
		double total_cost;
	};

	using path = std::vector<visited_node>;

	using cost_function_ptr = double (*)(void const* callback_data, from<double>, to<double>);

	using search_domain = dimensions_2d<int64_t,
		boundary_type::inclusive,
		boundary_type::exclusive,
		boundary_type::inclusive,
		boundary_type::exclusive>;

	path search_impl(from<int64_t> source,
		to<int64_t> target,
		search_domain const& domain,
		void const* callback_data,
		cost_function_ptr cost_function);

	template<class CostFunction = flat_euclidian_norm>
	auto search(from<int64_t> source,
		to<int64_t> target,
		search_domain const& domain,
		CostFunction&& f = flat_euclidian_norm{})
	{
		return search_impl(source, target, domain, &f, [](void const* func_pair,
			from<double> x0,
			to<double> x1){
			auto const& data = *static_cast<CostFunction const*>(func_pair);
			return static_cast<double>(data(x0, x1));
		});
	}
}

#endif