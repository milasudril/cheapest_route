//@	{"target":{"name":"cheapest_route.o"}}

#include "./cmdline.hpp"
#include "./io_utils.hpp"
#include "./scaling_factors.hpp"
#include "./image_loader.hpp"

#include "lib/search.hpp"
#include "pixel_store/image.hpp"

#include <cassert>

namespace cheapest_route
{
	float interp(pixel_store::image_span<float const> img, vec2f_t loc)
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


	struct flat_earth_distance_with_terrain
	{
		pixel_store::image_span<float const> image;
		scaling_factors scale;

		auto operator()(from<double> x1, to<double> x2) const
		{
			auto const dx = x2 - x1;
			auto const z1 = interp(image, x1.value());
			auto const z2 = interp(image, x2.value());

			auto const dr = scale*vec<float, 4>{static_cast<float>(dx[0]), static_cast<float>(dx[1]), z2 - z1, 0.0f};

			return std::sqrt(dot(dr, dr));
		}
	};
}

int main(int argc, char** argv) try
{
	cheapest_route::command_line cmdline{argc, argv};

	cheapest_route::from<int64_t> origin_loc{cmdline["origin"]};
	cheapest_route::to<int64_t> dest_loc{cmdline["destination"]};
	auto const scale = get_or(cmdline, "scaling_factors", cheapest_route::scaling_factors{1.0f, 1.0f, 1.0f});

	std::filesystem::path heighmap_path{cmdline["heightmap"]};
	auto const cost_function_path = get_if<std::filesystem::path>(cmdline, "cost_function");
	if(!cost_function_path.has_value())
	{
		fprintf(stderr, "(!) No cost_function set, using homogenous cost of 1\n");
	}

	auto const heightmap = cheapest_route::load_image(heighmap_path);
	auto const domain = cheapest_route::search_domain{
		static_cast<int64_t>(heightmap.width()), static_cast<int64_t>(heightmap.height())
	};

	auto const result = search(origin_loc,
		dest_loc,
		domain,
		cheapest_route::flat_earth_distance_with_terrain{heightmap.pixels(), scale});

	auto output_file =
		get_or<cheapest_route::output_file>(get_if<std::filesystem::path>(cmdline, "output.file"),
			cheapest_route::output_file{cheapest_route::std_output_stream{stdout}});
	cheapest_route::path_file_encoder const path_encoder{cmdline["output.format"]};
	cheapest_route::length_unit const output_lu{cmdline["output.length_unit"]};

	path_encoder(output_file.get(), result, length_unit, scale);
}
catch(std::exception const& err)
{
	fprintf(stderr, "cheapest_route: %s\n", err.what());
	return -1;
}
