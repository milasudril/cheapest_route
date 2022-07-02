//@	{"target":{"name":"search.o"}}

#include "./search.hpp"

#include <vector>
#include <queue>
#include <cmath>
#include <numbers>
#include <array>
#include <memory>

namespace cheapest_route
{
	struct pending_route_node
	{
		to<int64_t> loc;
		double total_cost;
	};

	bool is_cheaper(pending_route_node const& a, pending_route_node const& b)
	{
		return a.total_cost < b.total_cost;
	}

	struct route_node
	{
		from<int64_t> loc;
		double total_cost;

		route_node():loc{}, total_cost{std::numeric_limits<double>::infinity()}{}
	};

	constexpr auto scale = 4.0;
	constexpr auto scale_int = static_cast<int64_t>(scale);

	constexpr auto gen_neigbour_offset_table()
	{
		std::array<to<int64_t>, 32> ret{};
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

	struct node:public route_node  // Inherit from node to save some space
	{
		bool visited{false};
	};

	auto do_search(from<int64_t> source,
		to<int64_t> target,
		dimensions_2d<int64_t, boundary_type::inclusive, boundary_type::exclusive, boundary_type::inclusive, boundary_type::exclusive> const& domain,
		void const* callback_data,
		cost_function_ptr cost_function)
	{
		if(domain.width() < 1 || domain.height() < 1)
		{ std::runtime_error{"Empty search domain"}; }

		if(outside(vec<int64_t, 2>{source}, domain))
		{ std::runtime_error{"Source location is outside search domain"}; }

		if(outside(vec<int64_t, 2>{target}, domain))
		{ std::runtime_error{"Target location is outside search domain"}; }

		auto cmp = [](pending_route_node const& a, pending_route_node const& b)
		{ return is_cheaper(b, a); };

		std::priority_queue<pending_route_node, std::vector<pending_route_node>, decltype(cmp)> nodes_to_visit;
		nodes_to_visit.push(pending_route_node{scale_int*to<int64_t>{source}, 0.0});

		auto const dom_scaled = scale_int*domain - vec<int64_t, 2>{scale_int - 1, scale_int - 1};

		auto const w = dom_scaled.width();
		auto const h = dom_scaled.height();

		auto cost_table = std::make_unique<node[]>(w*h);

		while(!nodes_to_visit.empty())
		{
			auto current = nodes_to_visit.top();
			nodes_to_visit.pop();
			auto& cost_item = get_item(cost_table.get(), current.loc, w);
			cost_item.visited = true;

			auto const from_loc = from<int64_t>{current.loc};
			auto const from_loc_scaled = scale_to_float(scale, from_loc);

			if(length_squared(to<double>{target} - from_loc_scaled) < 1.0)
			{ return std::pair{std::move(cost_table), dom_scaled}; }

			for(auto item : neigbour_offsets)
			{
 				auto const next_loc = current.loc + item;
				if(outside(vec<int64_t, 2>(next_loc), dom_scaled))
				{ continue; }

				auto const next_scaled = scale_to_float(scale, next_loc);
				auto const dx = next_scaled - from_loc_scaled;
				vec<double, 2, quantity_type::point> const x{0.5*(
					vec<double, 2>{next_scaled} + vec<double, 2>{from_loc_scaled})
				};
				auto const cost_increment = cost_function(callback_data, dx, x);

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
					new_cost_item.loc = from<int64_t>{current.loc.value()};
					nodes_to_visit.push(pending_route_node{next_loc, new_cost});
				}
			}
		}
		throw std::runtime_error{std::string{"Target "}.append(to_string(target)).append(" not reached")};
	}

	template<class T>
	auto follow_path(T const& cost_table, to<int64_t> target)
	{
		auto loc_search = static_cast<from<int64_t>>(scale_int*target);
		path ret;
		size_t k = 0;
		while(true)
		{
			auto const loc = scale_to_float(scale, loc_search);
			auto const& item = get_item(cost_table.first.get(), loc_search, cost_table.second.width());
			if(k % scale_int == 0 || item.total_cost == std::numeric_limits<double>::infinity())
			{
				ret.push_back(path::value_type{vec<double, 2, quantity_type::point>{loc}, item.total_cost});
			}
			if(item.total_cost == std::numeric_limits<double>::infinity())
			{ return ret;}
			loc_search = item.loc;
			++k;
		}
		return ret;
	}

	path search_impl(from<int64_t> source,
		to<int64_t> target,
		dimensions_2d<int64_t, boundary_type::inclusive, boundary_type::exclusive, boundary_type::inclusive, boundary_type::exclusive> const& domain,
		void const* callback_data,
		cost_function_ptr cost_function)
	{
		auto tmp = do_search(source, target, domain, callback_data, cost_function);
		return follow_path(tmp, target);
	}
}