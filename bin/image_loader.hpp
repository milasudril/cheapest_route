#ifndef CHEAPESTROUTE_IMAGELOADER_HPP
#define CHEAPESTROUTE_IMAGELOADER_HPP

#include "pixel_store/image.hpp"
#include <filesystem>

namespace cheapest_route
{
	using image_type = pixel_store::image<float>;

	image_type load_image(std::filesystem::path const& filename);
}
#endif