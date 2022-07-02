//@	{"target":{"name":"search.test"}}

#include "./search.hpp"
#include "./rectangle.hpp"

#include <chrono>

int main()
{
	int64_t const size = 1024;
	auto const valid_range =
		cheapest_route::make_interval<cheapest_route::boundary_type::inclusive,
			cheapest_route::boundary_type::exclusive>(0l, size);
	auto const rect = cheapest_route::rectangle{valid_range, valid_range}.dimensions();
#if 1
	auto result1 = search(cheapest_route::from<int64_t>{size - 1, 2*size/3},
		cheapest_route::to<int64_t>{0, size/3}, rect);
#endif
#if 0
	auto result2 = search(cheapest_route::from<int64_t>{size - 1, 2*size/3},
		cheapest_route::to<int64_t>{0, size/3}, cost_function);
#endif

//	printf("%.8e\n", std::chrono::duration<double>{std::chrono::steady_clock::now() - t0}.count());
//	follow_path(result1, cheapest_route::to<int64_t>{size - 1, 2*size/3});
	follow_path(result1, cheapest_route::to<int64_t>{0, size/3});
}