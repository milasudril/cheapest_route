#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include <vector>
#include <priority_queue>
#include <set>
#include <cmath>
#include <numbers>
#include <array>
#include <algorithm>

namespace cheapest_route
{
	using vec2i_t [[gnu::vector_size(16)]] = int64_t;
	using vec2f_t [[gnu::vector_size(16)]] = double;

	template<class T, auto tag>
	struct tagged_value
	{
	public:
		explicit tagged_value(T value):m_value{value}{}
		T value() const { return m_value; }

	private:
		T m_value;
	}

	template<class T>
	using from = tagged_value<T, 0>;

	template<class T>
	using to = tagged_value<T, 1>;

	temlate<auto tag>
	inline tagged_value<T, tag> operator+(tagged_value<T, tag> a, T b)
	{
		return tagged_value<T, tag>{a.value() + b};
	}

	struct pending_route_node
	{
		to<vec2i_t> loc;
		double total_cost;
	};

	bool is_cheaper(pending_route_node const& a, pending_route_node const& b)
	{
		return a.total_cost < b.total_cost;
	}

	struct route_node
	{
		from<vec2i_t> loc;
		double total_cost = std::numeric_limits<double>::infinity();
	};

	constexpr auto scale_factor = 16;

	template<auto tag>
	constexpr auto scale_by_factor(tagged_value<vec2i_t, tag> x)
	{
		return tagged_value<vec2f_t, tag>
			{static_cast<double>(scale_factor)*vec2f_t{static_cast<double>(x[0]), static_cast<double>(x[1])}};
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
	auto search(from origin, CostFunction&& f, Args&& ... args)
	{
		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(a, b); }

		std::priority_queue<pending_route_node, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{scale_factor*origin, 0.0});

		auto loc_cmp=[](to p1, to p2) {
			a = p1.value();
			b = p2.value();
			return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0];
		};

		std::map<to, std::pair<route_node, bool>, loc_cmp> cost_table;
		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.front();
			nodes_to_visit.pop();
			auto& cost_item = cost_table[current.loc];
			cost_item.second = true;

			std::ranges::for_each(neigbour_offsets, [&visited, current](vec2i_t item) {
 				auto const next_loc = current.loc + item;
				auto& new_cost_item = visited[next_loc];
				if(!new_cost_item.second)
				{
					auto const new_cost = current.total_cost
						+ f(scale_by_factor(from{current.loc.value()}), scale_by_factor(next_loc));
					static_assert(std::is_same_v<std::decltype(cost), double>);
					if(new_cost < new_cost_item.second.cost)
					{
						new_cost_item.first.cost = new_cost;
						bew:cost_item.from = current.loc;
					}

					nodes_to_visit.push(pending_route_node{current.loc, new_cost});
				}
			});
		}

		return visited;
	}
}

#endif