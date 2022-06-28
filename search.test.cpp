//@	{"target":{"name":"search.test"}}

#include "./search.hpp"

int main()
{
	int64_t const size = 1024;
	auto result = search(cheapest_route::from<int64_t>{-size, -size/2},
						 cheapest_route::to<int64_t>{size, size/2},
		[size = static_cast<double>(size)](cheapest_route::from<double> a, cheapest_route::to<double> b) {

		if(std::max(std::abs(a[0]), std::abs(a[1])) > size
			|| std::max(std::abs(b[0]), std::abs(b[1])) > size)
		{ return std::numeric_limits<double>::infinity(); }

		auto const ret = std::sqrt(length_squared(a - cheapest_route::from<double>{b}));;

	//	printf("(%.8g %.8g) -> (%.8g %.8g) %.8g\n", a[0], a[1], b[0], b[1], ret);

		return ret;
	});

	follow_path(result, cheapest_route::to<int64_t>{size, size/2});
# if 0
	std::ranges::for_each(result, [](auto const& item) {
		auto [to, node_info] = item;
		printf("to = (%ld, %ld), from = (%ld, %ld). total_cost = %.8g\n",
			   to.value()[0], to.value()[1],
			   node_info.first.loc.value()[0], node_info.first.loc.value()[1],
			   node_info.first.total_cost);
	});
#endif
}