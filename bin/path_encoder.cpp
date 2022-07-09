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
		cheapest_route::path const& nodes,
		float const*)
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
		cheapest_route::scaling_factors world_scale,
		cheapest_route::search_domain,
		cheapest_route::path const& nodes,
		float const* elevation_profile
	)
	{
		for(size_t k = 0 ; k != std::size(nodes); ++k)
		{
			auto const& item = nodes[k];
			auto const loc_scaled = world_scale*cheapest_route::vec<float, 4>{static_cast<float>(item.loc[0]),
				static_cast<float>(item.loc[1]),
				elevation_profile[k],
				0.0f};
			fprintf(f, "%.8e %.8e %.8e %.8e\n", loc_scaled[0], loc_scaled[1], loc_scaled[2], item.integrated_cost);
		}
	}

	void encode_json(FILE* f,
		cheapest_route::length_unit lu,
		cheapest_route::scaling_factors world_scale,
		cheapest_route::search_domain,
		cheapest_route::path const& nodes,
		float const* elevation_profile)
	{
		std::string x_vals;
		std::string y_vals;
		std::string z_vals;
		std::string integrated_cost;

		auto gen_item = [](size_t k, double value) {
			char buffer[20];
			sprintf(buffer, "%.8e", value);
			return std::string{k == 0 ? "" : ", "}.append(buffer);
		};

		for(size_t k = 0; k!= std::size(nodes); ++k)
		{
			x_vals += gen_item(k, nodes[k].loc[0]);
			y_vals += gen_item(k, nodes[k].loc[1]);
			z_vals += gen_item(k, elevation_profile[k]);
			integrated_cost += gen_item(k, nodes[k].integrated_cost);
		}

		fprintf(f, R"json({
	"length_unit": "%s",
	"world_scale": "%.8e %.8e %.8e %.8e",
	"path": {
		"x": [%s],
		"y": [%s],
		"z": [%s],
		"integrated_cost": [%s]
	}
"})json",
			lu.name(),
			world_scale.x(), world_scale.y(), world_scale.z(), 0.0,
			x_vals.c_str(),
			y_vals.c_str(),
			z_vals.c_str(),
			integrated_cost.c_str()
		);
	}

}

cheapest_route::path_encoder::path_encoder(std::string_view str)
{
	if(str == "svg")
	{ encode = encode_svg; }
	else
	if(str == "txt")
	{ encode = encode_txt; }
	else
	if(str == "json")
	{ encode = encode_json; }
	else
	{
		throw std::runtime_error{"Unsupported output format"};
	}
}
