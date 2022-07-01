//@	{"target":{"name":"search.test"}}

#include "./search.hpp"

#include <chrono>

int main()
{
	int64_t const size = 1024;
	auto const t0 = std::chrono::steady_clock::now();
	auto result = search(cheapest_route::from<int64_t>{0, size/4},
						 cheapest_route::to<int64_t>{size - 1, 3*size/4},
		[size = static_cast<double>(size)](cheapest_route::from<double> a, cheapest_route::to<double> b) {

		if(a[0] < 0 || a[1] >= size || b[0] < 0 || b[1] >= size)
		{ return std::numeric_limits<double>::infinity(); }

		auto const ret = std::sqrt(length_squared(a - cheapest_route::from<double>{b}));;

	//	printf("(%.8g %.8g) -> (%.8g %.8g) %.8g\n", a[0], a[1], b[0], b[1], ret);

		return ret;
	});

	printf("%.8e\n", std::chrono::duration<double>{std::chrono::steady_clock::now() - t0}.count());
#if 0
	follow_path(result, cheapest_route::to<int64_t>{size - 1, 3*size/4});
	std::ranges::for_each(result, [](auto const& item) {
		auto [to, node_info] = item;
		printf("to = (%ld, %ld), from = (%ld, %ld). total_cost = %.8g\n",
			   to.value()[0], to.value()[1],
			   node_info.first.loc.value()[0], node_info.first.loc.value()[1],
			   node_info.first.total_cost);
	});
#endif
}