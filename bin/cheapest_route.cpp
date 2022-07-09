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

	void print_help()
	{
		printf(R"text(Usage: cheapest_route [options]

`cheapest_route` finds the cheapest route between two points, given a cost map stored
in an image.

Options are key-value pairs written on the form key=value. Depending on your shell, you
may need to escape `(`  and `)` with `\(` and `\)`.

Example command:

```bash
cheapest_route cost_map=test.exr \
	origin=\(0,1103\) \
	destination=\(1626,303\) \
	world_scale=\(25.41351674641148,31.31489234449761,10000\) \
	output_format=svg length_unit=m output_file=/dev/shm/test.svg \
	friction_strength=0.01 wind_strength=\(393.4913888457429,319.3368793987605\)
```

Supported options are

+----------------------+---------------+----------------------------------------------------+
| Option               | Default value | Description                                        |
+======================+===============+====================================================+
| origin=(x,y)         | *mandatory*   | Sets the starting point of the path                |
+----------------------+---------------+----------------------------------------------------+
| destination=(x,y)    | *mandatory*   | Sets the final point of the path                   |
+----------------------+---------------+----------------------------------------------------+
| world_scale=(x,y,z)  | (1,1,1)       | Sets scaling factors for x, y, and z values.       |
|                      |               | x and y affects the size of the domain, while z    |
|                      |               | scales the intensity of each pixel.                |
+----------------------+---------------+----------------------------------------------------+
| friction_strength=µ  | 1             | The "friction" strength. Friction determines the   |
|                      |               | cost of passing an individual pixel.               |
+----------------------+---------------+----------------------------------------------------+
| wind_strength=(x,y)  | (1,1)         | The "wind" strength. Wind deterines how easy/hard  |
|                      |               | it is to travel in a particular direction.         |
+----------------------+---------------+----------------------------------------------------+
| cost_map=file.exr    | *mandatory*   | The image file that contains the cost map. A       |
|                      |               | grayscale image is used as a pure heighmap. In     |
|                      |               | this case the "friction" will be constant, and     |
|                      |               | there will be no "wind". It is also possible to    |
|                      |               | use a RGBA image. In this case                     |
|                      |               | - red is mapped to the local elevation             |
|                      |               | - green is mapped to the local friction            |
|                      |               | - blue is the x component of the wind              |
|                      |               | - alpha is the y component of the wind             |
+----------------------+---------------+----------------------------------------------------+
| output_format=fmt    | *mandatory*   | Selects how to export serialize the resulting      |
|                      |               | path. Supported formats are                        |
|                      |               | - json - exports the result as well as scaling     |
|                      |               |         factors and field strengths                |
|                      |               | - txt - saves the path as columns with x, y, z,    |
|                      |               |         and the integrated cost. Use this format   |
|                      |               |         if you which to import the results using   |
|                      |               |         `load` in Octave.                          |
|                      |               | - svg - saves the path as an svg file. This is     |
|                      |               |         only saves the path projected onto the xy  |
|                      |               |         plane, not any information about the       |
|                      |               |         elevation or the cost. This is useful if   |
|                      |               |         you want to import the path into some kind |
|                      |               |         of graphics software such as Inkscape or   |
|                      |               |         Blender.                                   |
+----------------------+---------------+----------------------------------------------------+
| length_unit=unit     | *mandatory*   | Sets the unit of length for world_scale. Valid     |
|                      |               | units are                                          |
|                      |               | - m                                                |
|                      |               | - dm                                               |
|                      |               | - cm                                               |
|                      |               | - mm                                               |
|                      |               | - in                                               |
|                      |               | - svg                                              |

)text");
	}
}

int main(int argc, char** argv) try
{
	cheapest_route::command_line cmdline{argc, argv};
	if(std::size(cmdline) == 0)
	{ throw std::runtime_error{"Try cheapest_route help="}; }

	if(cmdline.contains("help"))
	{
		if(std::size(cmdline) != 1)
		{ throw std::runtime_error{"Try cheapest_route help="}; }

		cheapest_route::print_help();
		return 0;
	}

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
