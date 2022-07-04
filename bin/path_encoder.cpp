//@	{"target":{"name":"./path_encoder.o"}}

#include "./path_encoder.hpp"

#include <stdexcept>
#include <algorithm>

namespace
{
	void encode_svg(FILE* f,
		cheapest_route::length_unit lu,
		cheapest_route::scaling_factors factors,
		cheapest_route::search_domain domain,
		cheapest_route::path const& nodes)
	{
		using cheapest_route::vec4f_t;
		auto const unit_factor = 90.0*lu.factor()/0.0254;

		auto const dom_scaled =
			 static_cast<float>(unit_factor)
			* factors.values()*vec4f_t{static_cast<float>(domain.width()), static_cast<float>(domain.height()), 0.0f, 0.0f}
			+ vec4f_t{0.5f, 0.5f, 0.0f, 0.0f};

		fprintf(f,R"xml(<svg width="%d" height="%d" xmlns="http://www.w3.org/2000/svg">
<g transform="scale(%.8e %.8e)">
<polyline stroke="blue" points=")xml",
			static_cast<int>(dom_scaled[0]), static_cast<int>(dom_scaled[1]),
			factors.x(), factors.y()
		);
		std::ranges::for_each(nodes, [f, unit_factor](auto const& item){
			auto const val = unit_factor*cheapest_route::vec<double, 2>{item.loc};
			fprintf(f, "%.8e,%.8e ", val[0], val[1]);
		});
		fputs(R"(" fill="none"/>
</g>
</svg>)", f);
	}

	void encode_txt(FILE* f,
		cheapest_route::length_unit,
		cheapest_route::scaling_factors,
		cheapest_route::search_domain,
		cheapest_route::path const& nodes)
	{
		// TODO: Apply scaling
		std::ranges::for_each(nodes, [f](auto const& item){
			fprintf(f, "%.8e %.8e %.8e\n", item.loc[0], item.loc[1], item.total_cost);
		});
	}
}

cheapest_route::path_encoder::path_encoder(std::string_view str)
{
	if(str == "svg")
	{ encode = encode_svg; }
	else
	if(str == "txt")
	{ encode = encode_txt; }
#if 0
	else
	if(str == "json")
	{ encode = encode_json; }
#endif
	else
	{
		throw std::runtime_error{"Unsupported output format"};
	}
}
