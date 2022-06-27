//@	{"target":{"name":"search.test"}}

#include "./search.hpp"

int main()
{
	auto const size = 16;
	auto result = search(cheapest_route::from{cheapest_route::vec2i_t{-size, -size}}, cheapest_route::to{cheapest_route::vec2i_t{size, size}},
		[size = static_cast<double>(size)](cheapest_route::from<cheapest_route::vec2f_t> a, cheapest_route::to<cheapest_route::vec2f_t> b) {
		auto const v1 = a.value();
		auto const v2 = b.value();

		if((v2[0] < -size || v2[0] > size) || (v2[1] < -size || v2[1] > size))
		{ return std::numeric_limits<double>::infinity(); }

		auto delta = v1 - v2;
		auto d2 = delta*delta;
		return d2[0] + d2[1];
	});

	std::ranges::for_each(result, [](auto const& item) {
		auto [to, node_info] = item;
		printf("to = (%ld, %ld), from = (%ld, %ld). total_cost = %.8g\n",
			   to.value()[0], to.value()[1],
			   node_info.first.loc.value()[0], node_info.first.loc.value()[1],
			   node_info.first.total_cost);
	});
}