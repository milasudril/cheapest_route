//@	{"target":{"name":"search.test"}}

#include "./search.hpp"
#include "./rectangle.hpp"

#include <chrono>

int main()
{
	int64_t const size = 1024;
	auto const valid_range =
		cheapest_route::make_interval<cheapest_route::boundary_type::inclusive,
			cheapest_route::boundary_type::exclusive>(0.0, static_cast<double>(size));
	auto const rect = cheapest_route::rectangle{valid_range, valid_range};
	auto cost_function = [rect](cheapest_route::from<double> a, cheapest_route::to<double> b) {
		if(outside(cheapest_route::vec<double, 2>{a}, rect)
			|| outside(cheapest_route::vec<double, 2>{b}, rect))
		{ return std::numeric_limits<double>::infinity(); }

		auto const ret = std::sqrt(length_squared(a - cheapest_route::from<double>{b}));
		return ret;
	};
#if 1
	auto result1 = search(cheapest_route::to<int64_t>{size - 1, 2*size/3},
		cheapest_route::from<int64_t>{0, size/3}, cost_function);
#endif
#if 0
	auto result2 = search(cheapest_route::from<int64_t>{size - 1, 2*size/3},
		cheapest_route::to<int64_t>{0, size/3}, cost_function);
#endif

//	printf("%.8e\n", std::chrono::duration<double>{std::chrono::steady_clock::now() - t0}.count());
	follow_path(result1, cheapest_route::to<int64_t>{size - 1, 2*size/3});
//	follow_path(result2, cheapest_route::to<int64_t>{0, size/3});
}