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

	auto const& channels = src.header().channels();

	auto const red = channels.findChannel("R");
	auto const green = channels.findChannel("G");
	auto const blue = channels.findChannel("B");
	auto const alpha = channels.findChannel("A");
	auto const luminance = channels.findChannel("Y");

	auto const has_rgba = red != nullptr && green != nullptr && blue != nullptr && alpha != nullptr;
	auto const has_luminance = luminance != nullptr;

	if(has_rgba && has_luminance)
	{
		throw std::runtime_error{"Ambigous channel set. Input image should use either RGBA or Y."};
	}

	if(!has_rgba && has_luminance)
	{
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

	if(has_luminance && !has_rgba)
	{
		throw std::runtime_error{"Unimplemented channel set."};
	}

	throw std::runtime_error{"Unsupported channel set. Input image should use either RGBA or Y."};
}