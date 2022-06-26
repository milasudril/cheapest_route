#ifndef CHEAPESTROUTE_SEARCH_HPP
#define CHEAPESTROUTE_SEARCH_HPP

#include <vector>
#include <queue>
#include <map>
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
	};

	template<class T>
	using from = tagged_value<T, 0>;

	template<class T>
	using to = tagged_value<T, 1>;

	template<class T, auto tag>
	inline tagged_value<T, tag> operator+(tagged_value<T, tag> a, T b)
	{
		return tagged_value<T, tag>{a.value() + b};
	}

	template< auto tag>
	inline tagged_value<int64_t, tag> operator*(int64_t b, tagged_value<vec2i_t, tag> a)
	{
		return tagged_value<int64_t, tag>{b*a.value()};
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
	auto search(from<vec2i_t> origin, CostFunction&& f, Args&& ... args)
	{
		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(a, b); };

		std::priority_queue<pending_route_node, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{to<vec2i_t>{scale_factor*origin.value()}, 0.0});

		auto loc_cmp=[](to<vec2i_t> p1, to<vec2i_t> p2) {
			auto const a = p1.value();
			auto const b = p2.value();
			return (a[0] == b[0]) ? a[1] < b[1] : a[0] < b[0];
		};

		std::map<to<vec2i_t>, std::pair<route_node, bool>, decltype(loc_cmp)> cost_table;
		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.front();
			nodes_to_visit.pop();
			auto& cost_item = cost_table[current.loc];
			cost_item.second = true;

			std::ranges::for_each(neigbour_offsets, [&cost_table, current, &f, &nodes_to_visit](vec2i_t item) {
 				auto const next_loc = current.loc + item;
				auto& new_cost_item = cost_table[next_loc];
				if(!new_cost_item.second)
				{
					auto const new_cost = current.total_cost
						+ f(scale_by_factor(from{current.loc.value()}), scale_by_factor(next_loc));
					static_assert(std::is_same_v<decltype(new_cost), double>);
					if(new_cost < new_cost_item.second.cost)
					{
						new_cost_item.first.cost = new_cost;
						bew:cost_item.from = current.loc;
					}

					nodes_to_visit.push(pending_route_node{current.loc, new_cost});
				}
			});
		}

		return cost_table;
	}
}

#endif