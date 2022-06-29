#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include "./vec.hpp"

#include <vector>
#include <queue>
#include <map>
#include <cmath>
#include <numbers>
#include <array>
#include <algorithm>
#include <ranges>
#include <random>

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

	constexpr auto gen_neigbour_offset_table()
	{
		std::array<to<int64_t>, 8> ret{};
		constexpr auto r = 1.0;
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			auto const theta = -k*2.0*std::numbers::pi/std::size(ret);
			auto const v = to<double>{std::round(r*std::cos(theta)), std::round(r*std::sin(theta))};
			ret[k] = to<int64_t>{v};
		}
		return ret;
	}

	constexpr auto neigbour_offsets = gen_neigbour_offset_table();

	template<class CostFunction>
	auto search(from<int64_t> source, to<int64_t> target, CostFunction&& f)
	{
		std::mt19937 rng;
		std::uniform_int_distribution<int64_t> U{-16, 16};

		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(b, a); };

		std::priority_queue<pending_route_node, std::vector<pending_route_node>, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{to<int64_t>{source}, rank{0.0, U(rng)}});

		auto loc_cmp=[](to<int64_t> p1, to<int64_t> p2) {
			auto const a = p1.value();
			auto const b = p2.value();
			return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0];
		};

		std::map<to<int64_t>, std::pair<route_node, bool>, decltype(loc_cmp)> cost_table;
		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.top();
			nodes_to_visit.pop();
			auto& cost_item = cost_table[current.loc];
			cost_item.second = true;

			if(length_squared(current.loc - to<int64_t>{target}) < 1.0)
			{
				return cost_table;
			}

			for(auto item : neigbour_offsets)
			{
 				auto const next_loc = current.loc + item;

				auto const cost_increment = f(from<int64_t>{current.loc}, next_loc);
				static_assert(std::is_same_v<std::decay_t<decltype(cost_increment)>, double>);
				if(cost_increment == std::numeric_limits<double>::infinity())
				{ break; }

				auto& new_cost_item = cost_table[next_loc];
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
		auto i = cost_table.find(target);
		while(i != std::end(cost_table))
		{
			printf("%ld %ld %.8g\n", i->first[0], i->first[1], i->second.first.r.total_cost);
			if(i->second.first.r.total_cost == std::numeric_limits<double>::infinity())
			{ return 0;}
			i = cost_table.find(i->second.first.loc);
		}
		return 0;
	}
}

#endif