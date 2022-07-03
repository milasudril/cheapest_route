//@	{"target":{"name":"search.o"}}

#include "./search.hpp"

#include <vector>
#include <queue>
#include <cmath>
#include <numbers>
#include <array>
#include <memory>

namespace
{
	constexpr auto scale = 4.0;
	constexpr auto scale_int = static_cast<int64_t>(scale);

	struct pending_route_node
	{
		cheapest_route::to<int64_t> loc;
		double total_cost;
	};

	bool is_cheaper(pending_route_node const& a, pending_route_node const& b)
	{
		return a.total_cost < b.total_cost;
	}

	struct route_node
	{
		cheapest_route::from<int64_t> loc;
		double total_cost;

		route_node():loc{}, total_cost{std::numeric_limits<double>::infinity()}{}
	};

	constexpr auto gen_neigbour_offset_table()
	{
		std::array<cheapest_route::to<int64_t>, 32> ret{};
		constexpr auto r = scale;
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			auto const theta = k*2.0*std::numbers::pi/std::size(ret);
			auto const v = cheapest_route::to<double>{std::round(r*std::cos(theta)), std::round(r*std::sin(theta))};
			ret[k] = cheapest_route::to<int64_t>{v};
		}
		return ret;
	}

	constexpr auto neigbour_offsets = gen_neigbour_offset_table();

	template<class T, auto tag>
	T& get_item(T* ptr, cheapest_route::vec<int64_t, 2, tag> loc, size_t width)
	{
		auto const x = loc[0];
		auto const y = loc[1];
		return *(ptr + y*width + x);
	}

	struct node:public route_node  // Inherit from node to save some space
	{
		bool visited{false};
	};

	struct search_result
	{
		std::unique_ptr<node[]> cost_table;
		cheapest_route::from<int64_t> termination_point;
		cheapest_route::search_domain dom_scaled;
	};

	auto do_search(cheapest_route::from<int64_t> source,
		cheapest_route::to<int64_t> target,
		cheapest_route::search_domain const& domain,
		void const* callback_data,
		cheapest_route::cost_function_ptr cost_function)
	{
		if(domain.width() < 1 || domain.height() < 1)
		{ std::runtime_error{"Empty search domain"}; }

		if(outside(cheapest_route::vec<int64_t, 2>{source}, domain))
		{ std::runtime_error{"Source location is outside search domain"}; }

		if(outside(cheapest_route::vec<int64_t, 2>{target}, domain))
		{ std::runtime_error{"Target location is outside search domain"}; }

		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(b, a); };

		std::priority_queue<pending_route_node, std::vector<pending_route_node>, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{scale_int*cheapest_route::to<int64_t>{source}, 0.0});

		auto const dom_scaled = scale_int*domain - cheapest_route::vec<int64_t, 2>{scale_int - 1, scale_int - 1};

		auto const w = dom_scaled.width();
		auto const h = dom_scaled.height();

		auto cost_table = std::make_unique<node[]>(w*h);

		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.top();
			nodes_to_visit.pop();
			auto& cost_item = get_item(cost_table.get(), current.loc, w);
			cost_item.visited = true;

			auto const from_loc = cheapest_route::from<int64_t>{current.loc};
			auto const from_loc_scaled = scale_to_float(scale, from_loc);

			if(length_squared(cheapest_route::to<double>{target} - from_loc_scaled) < 1.0/(scale*scale))
			{ return search_result{std::move(cost_table), from_loc, dom_scaled}; }

			for(auto item : neigbour_offsets)
			{
 				auto const next_loc = current.loc + item;
				if(outside(cheapest_route::vec<int64_t, 2>(next_loc), dom_scaled))
				{ continue; }

				auto const next_scaled = scale_to_float(scale, next_loc);
				auto const cost_increment = cost_function(callback_data, from_loc_scaled, next_scaled);

				if(cost_increment < 0.0)
				{ throw std::runtime_error{"Cost function must be positive"}; }

				if(cost_increment == std::numeric_limits<double>::infinity())
				{ continue; }

				auto& new_cost_item = get_item(cost_table.get(), next_loc, w);
				if(new_cost_item.visited)
				{ continue; }

				auto const new_cost = current.total_cost + cost_increment;
				if(new_cost < new_cost_item.total_cost)
				{
					new_cost_item.total_cost = new_cost;
					new_cost_item.loc = cheapest_route::from<int64_t>{current.loc.value()};
					nodes_to_visit.push(pending_route_node{next_loc, new_cost});
				}
			}
		}
		throw std::runtime_error{std::string{"Target "}.append(to_string(target)).append(" not reached")};
	}
}

namespace
{
	auto follow_path(search_result const& res)
	{
		auto loc_search = res.termination_point;
		auto loc_prev = scale_to_float(scale, loc_search);
		cheapest_route::path ret;
		while(true)
		{
			auto const loc = scale_to_float(scale, loc_search);
			auto const dx = elem_abs(loc - loc_prev);
			auto const& item = get_item(res.cost_table.get(), loc_search, res.dom_scaled.width());
			if(std::size(ret) == 0
				|| item.total_cost == std::numeric_limits<double>::infinity()
				|| std::max(dx[0], dx[1]) >= 1.0)
			{
				ret.push_back(cheapest_route::path::value_type{
					cheapest_route::vec<double, 2, cheapest_route::quantity_type::point>{loc}, item.total_cost
				});
			}

			if(item.total_cost == std::numeric_limits<double>::infinity())
			{
				ret.back().total_cost = 0.0;
				std::reverse(std::begin(ret), std::end(ret));
				return ret;
			}

			loc_prev = loc;
			loc_search = item.loc;
		}
		return ret;
	}
}

cheapest_route::path cheapest_route::search_impl(from<int64_t> source,
	to<int64_t> target,
	dimensions_2d<int64_t, boundary_type::inclusive, boundary_type::exclusive, boundary_type::inclusive, boundary_type::exclusive> const& domain,
	void const* callback_data,
	cost_function_ptr cost_function)
{
	auto tmp = do_search(source, target, domain, callback_data, cost_function);
	return follow_path(tmp);
}