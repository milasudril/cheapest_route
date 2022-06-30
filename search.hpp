#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include "./vec.hpp"

#include <vector>
#include <queue>
#include <cmath>
#include <numbers>
#include <array>
#include <algorithm>
#include <ranges>
#include <random>
#include <memory>

namespace cheapest_route
{
	enum class node_type : int{source, target};

	template<class T>
	using from = vec<T, 2, node_type::source>;

	template<class T>
	using to = vec<T, 2, node_type::target>;

	struct rank
	{
		double total_cost;
		int64_t tiebreaker;

		auto operator<=>(rank const&) const = default;
	};

	struct pending_route_node
	{
		to<int64_t> loc;
		rank r;
	};

	bool is_cheaper(pending_route_node const& a, pending_route_node const& b)
	{
		return a.r < b.r;
	}

	struct route_node
	{
		from<int64_t> loc;
		rank r;

		route_node():loc{}, r{std::numeric_limits<double>::infinity(), 0}{}
	};

	constexpr auto scale = 2.0;
	constexpr auto scale_int = static_cast<int64_t>(scale);

	constexpr auto gen_neigbour_offset_table()
	{
		std::array<to<int64_t>, 16> ret{};
		constexpr auto r = scale;
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			auto const theta = k*2.0*std::numbers::pi/std::size(ret);
			auto const v = to<double>{std::round(r*std::cos(theta)), std::round(r*std::sin(theta))};
			ret[k] = to<int64_t>{v};
		}
		return ret;
	}

	constexpr auto neigbour_offsets = gen_neigbour_offset_table();

	template<class T, auto tag>
	T& get_item(T* ptr, vec<int64_t, 2, tag> loc, size_t width)
	{
		auto const x = loc[0];
		auto const y = loc[1];
		return *(ptr + y*width + x);
	}


	template<class CostFunction>
	auto search(from<int64_t> source, to<int64_t> target, CostFunction&& f)
	{
		std::mt19937 rng;
		std::uniform_int_distribution<int64_t> U{-16*scale_int, 16*scale_int};

		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(b, a); };

		std::priority_queue<pending_route_node, std::vector<pending_route_node>, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{scale_int*to<int64_t>{source}, rank{0.0, U(rng)}});

		constexpr auto w = scale_int*1024;
		constexpr auto h = scale_int*1024;

		auto cost_table = std::make_unique<std::pair<route_node, bool>[]>(w*h);

		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.top();
			nodes_to_visit.pop();
			auto& cost_item = get_item(cost_table.get(), current.loc, w);
			cost_item.second = true;

			if(length_squared(scale_to_float(scale, current.loc) - to<double>{target}) < 1.0)
			{/*
				auto const scaled = scale_to_float(scale, current.loc);
				printf("Exit: current=%ld %ld, current_scaled=%.8g %.8g, target=%ld %ld, distnace=%.8g\n",
					   current.loc[0], current.loc[1],
					   scaled[0], scaled[1],
					   target[0], target[1],
				       length_squared(scaled - to<double>{target}));
			*/
				return cost_table;
			}

			for(auto item : neigbour_offsets)
			{
 				auto const next_loc = current.loc + item;

				auto const cost_increment = f(scale_to_float(scale, from<int64_t>{current.loc}),
											  scale_to_float(scale, next_loc));
				static_assert(std::is_same_v<std::decay_t<decltype(cost_increment)>, double>);
				if(cost_increment == std::numeric_limits<double>::infinity())
				{ break; }

				auto& new_cost_item = get_item(cost_table.get(), next_loc, w);
				if(new_cost_item.second)
				{ break; }

				rank const new_rank{current.r.total_cost + cost_increment, current.r.tiebreaker + U(rng)};
				if(new_rank < new_cost_item.first.r)
				{
					new_cost_item.first.r = new_rank;
					new_cost_item.first.loc = from<int64_t>{current.loc.value()};
					nodes_to_visit.push(pending_route_node{next_loc, new_rank});
				}
			}
		}

		return cost_table;
	}

	template<class T>
	auto follow_path(T const& cost_table, to<int64_t> target)
	{
		auto loc_search = scale_int*target;
		while(true)
		{
			auto const loc = scale_to_float(scale, loc_search);
			auto const& item = get_item(cost_table.get(), loc_search, 2*1024);
			printf("%.8g %.8g %.8g\n", loc[0], loc[1], item.first.r.total_cost);
			if(item.first.r.total_cost == std::numeric_limits<double>::infinity())
			{ return 0;}
			loc_search = item.first.loc;
		}
		return 0;
	}
}

#endif