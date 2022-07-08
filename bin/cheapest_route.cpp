//@	{"target":{"name":"cheapest_route.o"}}

#include "./cmdline.hpp"
#include "./io_utils.hpp"
#include "./scaling_factors.hpp"
#include "./image_loader.hpp"
#include "./path_encoder.hpp"
#include "./length_unit.hpp"

#include "lib/search.hpp"
#include "pixel_store/image.hpp"

#include <cassert>

namespace cheapest_route
{
	cost_values interp(pixel_store::image_span<cost_values const> img, vec2f_t loc)
	{
		auto const x_0  = static_cast<int64_t>(loc[0]);
		auto const y_0  = static_cast<int64_t>(loc[1]);

		auto const w = static_cast<int64_t>(img.width());
		auto const h = static_cast<int64_t>(img.height());

		auto const x_1  = std::min(x_0 + 1, w - 1);
		auto const y_1  = std::min(y_0 + 1, h - 1);

		auto const z_00 = img(x_0, y_0);
		auto const z_01 = img(x_0, y_1);
		auto const z_10 = img(x_1, y_0);
		auto const z_11 = img(x_1, y_1);

		auto const xi = loc - vec2f_t{static_cast<double>(x_0), static_cast<double>(y_0)};

		auto const z_x0 = (1.0f - static_cast<float>(xi[0])) * z_00 + static_cast<float>(xi[0]) * z_10;
		auto const z_x1 = (1.0f - static_cast<float>(xi[0])) * z_01 + static_cast<float>(xi[0]) * z_11;
		return (1.0f - static_cast<float>(xi[1])) * z_x0 + static_cast<float>(xi[1]) * z_x1;
	}

	struct cost_function
	{
		pixel_store::image_span<cost_values const> image;
		scaling_factors world_scale;
		float friction_strength;
		cheapest_route::vec<double, 2, cheapest_route::quantity_type::vector> wind_strength;

		auto operator()(from<double> x1, to<double> x2) const
		{
			auto const dx = x2 - x1;
			auto const c1 = interp(image, x1.value());
			auto const c2 = interp(image, x2.value());

			auto const dr = world_scale*vec<float, 4>{static_cast<float>(dx[0]),
				static_cast<float>(dx[1]),
				c2.elevation() - c1.elevation(),
				0.0f};

			auto const c = interp(image, midpoint(x2, x1).value());
			return friction_strength*c.friction()*std::sqrt(dot(dr, dr))
				+ std::abs(dot(scale(c.wind(), wind_strength), dx));
		}
	};
}

int main(int argc, char** argv) try
{
	cheapest_route::command_line cmdline{argc, argv};

	cheapest_route::from<int64_t> origin_loc{cmdline["origin"]};
	cheapest_route::to<int64_t> dest_loc{cmdline["destination"]};
	auto const world_scale = get_or(cmdline, "world_scale", cheapest_route::scaling_factors{1.0f, 1.0f, 1.0f});

	auto const friction_strength = get_or(cmdline, "friction_strength", 1.0f);
	if(friction_strength <= 0.0f)
	{ throw std::runtime_error{"The friction strength must be strictly positive"}; }

	auto const wind_strength = get_or(cmdline, "wind_strength",
									  cheapest_route::vec<double, 2, cheapest_route::quantity_type::vector>{1.0, 1.0});

	std::filesystem::path cost_map_path{cmdline["cost_map"]};
	auto const cost_map = cheapest_route::load_image(cost_map_path);
	auto const domain = cheapest_route::search_domain{
		static_cast<int64_t>(cost_map.width()), static_cast<int64_t>(cost_map.height())
	};

	cheapest_route::path_encoder const encode{cmdline["output_format"]};
	cheapest_route::length_unit const lu{cmdline["length_unit"]};

	auto const result = search(origin_loc,
		dest_loc,
		domain,
		cheapest_route::cost_function{cost_map.pixels(), world_scale, friction_strength, wind_strength});

	auto output_file =
		get_or<cheapest_route::output_file>(get_if<std::filesystem::path>(cmdline, "output_file"),
			cheapest_route::output_file{cheapest_route::std_output_stream{stdout}});

	encode(output_file.get(), lu, world_scale, domain, result);
}
catch(std::exception const& err)
{
	fprintf(stderr, "cheapest_route: %s\n", err.what());
	return -1;
}
