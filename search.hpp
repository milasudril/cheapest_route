#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include <vector>
#include <priority_queue>
#include <set>
#include <cmath>
#include <numbers>
#include <array>

namespace cheapest_route
{
	using vec2i_t [[gnu::vector_size(16)]] = int64_t;
	using vec2f_t [[gnu::vector_size(16)]] = double;

	struct route_node
	{
		vec2i_t loc;
		vec2i_t from_loc;
		double total_cost;
	};

	bool is_cheaper(route_node const& a, route_node const& b)
	{
		return a.total_cost < b.total_cost;
	}

	using path = std::vector<route_node>;

	constexpr auto scale_factor = 16;

	constexpr auto scale_by_factor(vec2i_t x)
	{
		return static_cast<double>(scale_factor)
			*vec2f_t{static_cast<double>(x[0]), static_cast<double>(x[1])};
	}

	constexpr auto gen_neigbour_offset_table()
	{
		std::array<vec2i_t, 24> ret{};
		constexpr auto r = static_cast<double>(scale_factor);
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			auto const theta = k*2.0*std::numbers::pi/std::size(ret);
			ret[k] = vec2i_t{static_cast<int64_t>(std::round(r*std::cos(theta))),
				static_cast<int64_t>(std::round(r*std::sin(theta)))};
		}
		return ret;
	}

	constexpr auto neigbour_offsets = gen_neigbour_offset_table();

	template<class CostFunction, class ... Args>
	path search(vec2i_t origin, CostFunction&& f, Args&& ... args)
	{
		auto cmp = [](route_node const& a, route_node const& b)
		{ return is_cheaper(a, b); }

		std::priority_queue<route_node, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(route_node{scale_factor*origin, vec2i_t{-1, -1}, 0.0});

		auto loc_cmp=[](vec2i_t a, vec2i_t b)
		{ return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0]; }

		std::set<vec2i_t, loc_cmp> visited;
		std::map<vec2f_t, double, loc_cmp> cost;
		path ret;
		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.front();
			nodes_to_visit.pop();
			visited.insert(current.loc);
			ret.push_back(current);
			std::ranges::for_each(neigbour_offsets, [&visited, current](vec2i_t item) {
				auto const next_loc = item + current.loc;
				if(!visited.contains(next_node))
				{
					auto const new_cost = current.total_cost + f(scale_by_factor(current.loc), scale_by_factor(next_loc));
					static_assert(std::is_same_v<std::decltype(cost), double>);
					if(new_cost < cost[next_loc])
					{ cost[next_loc] = new_cost; }

					nodes_to_visit.push(route_node{next_loc, current.loc, new_cost});
				}
			});
		}
	}
}

#endif