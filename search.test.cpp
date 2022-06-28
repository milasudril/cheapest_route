//@	{"target":{"name":"search.test"}}

#include "./search.hpp"

int main()
{
	int64_t const size = 4;
	auto result = search(cheapest_route::from<int64_t>{-size, -size},
						 cheapest_route::to<int64_t>{size, size},
		[size = static_cast<double>(size)](cheapest_route::from<double> a, cheapest_route::to<double> b) {

		if(std::max(std::abs(a[0]), std::abs(a[1])) > size
			|| std::max(std::abs(b[0]), std::abs(b[1])) > size)
		{ return std::numeric_limits<double>::infinity(); }

		return std::sqrt(length_squared(a - cheapest_route::from<double>{b}));
	});

	follow_path(result, cheapest_route::to<int64_t>{size, size});
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