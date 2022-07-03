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

		fprintf(f,R"(<svg width="%d" height="%d" xmlns="http://www.w3.org/2000/svg">
<g transform="scale(%.8e %.0e)>"
<polyline points="
)",
			static_cast<int>(dom_scaled[0]), static_cast<int>(dom_scaled[1]),
			factors.x(), factors.y()
		);
		std::ranges::for_each(nodes, [f, unit_factor](auto const& item){
			auto const val = unit_factor*cheapest_route::vec<double, 2>{item.loc};
			fprintf(f, "%.8e,%.8e ", val[0], val[1]);
		});
		fputs(R"( fill="none"/>
</g>
</svg>
)", f);
	}

#if 0

<svg width="10000" height="10000" xmlns="http://www.w3.org/2000/svg">
  <!--polyline points="100,100 150,25 150,75 200,0" stroke="black" fill="none"/-->
<g transform="scale(0.5 0.5)">
	<!--coordinates should be multiplied by 90/0.0254 for meters -->
<polyline points="3543.30709,3543.30709   5314.96063,885.82677 5314.96063,2657.48031 7086.61417,0.00000" fill="none"/>
</g>
</svg>

#endif
}

cheapest_route::path_encoder::path_encoder(std::string_view str)
{
	if(str == "svg")
	{ encode = encode_svg; }
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
