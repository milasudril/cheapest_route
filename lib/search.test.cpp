//@	{"target":{"name":"search.test"}}

#include "./search.hpp"

#include <algorithm>

int main()
{
	int64_t const size = 1024;
	auto const valid_range =
		cheapest_route::make_interval<cheapest_route::boundary_type::inclusive,
			cheapest_route::boundary_type::exclusive>(0l, size);
	auto const rect = cheapest_route::rectangle{valid_range, valid_range}.dimensions();

	auto result = search(cheapest_route::from<int64_t>{size - 1, 2*size/3},
		cheapest_route::to<int64_t>{0, size/3}, rect);

	std::ranges::for_each(result, [](auto const& item){
		printf("%.8g %.8g %.8g\n", item.loc[0], item.loc[1], item.integrated_cost);
	});
}