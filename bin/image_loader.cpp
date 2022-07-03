//@	{"target":{"name":"image_loader.o", "pkgconfig_libs":["OpenEXR"]}}

#include "./image_loader.hpp"

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfTestFile.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>

cheapest_route::image_type cheapest_route::load_image(std::filesystem::path const& filename)
{
	Imf::InputFile src{filename.c_str()};

	auto box = src.header().dataWindow();

	auto w = box.max.x - box.min.x + 1;
	auto h = box.max.y - box.min.y + 1;

	constexpr auto elem_size = sizeof(float);

	image_type ret{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	Imf::FrameBuffer fb;
	fb.insert("Y",
			Imf::Slice{Imf::FLOAT,
						(char*)(ret.pixels().data()) + 0 * sizeof(float),
						elem_size,
						elem_size * w});

	src.setFrameBuffer(fb);
	src.readPixels(box.min.y, box.max.y);

	return ret;
}